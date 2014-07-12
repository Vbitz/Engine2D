#include "Package.hpp"

#include "vendor/zlib123/zlib.h"

namespace Engine {
    // For performance reasons right now Package does not use PHYSFS and insteed uses memory mapped io
    
    uint32_t localOffsetToRegion(uint32_t localOffset) {
        uint32_t rem = abs(localOffset) % PACKAGE_REGION_SIZE;
        if (rem == 0) return localOffset;
        return localOffset + (PACKAGE_REGION_SIZE - rem) - PACKAGE_REGION_SIZE;
    }
    
    uint32_t localOffsetToRegionOffset(uint32_t localOffset) {
        return abs(localOffset) % PACKAGE_REGION_SIZE;
    }
    
    uint32_t roundSizeToRegionSize(uint32_t dataSize) {
        uint32_t rem = abs(dataSize) % PACKAGE_REGION_SIZE;
        if (rem == 0) return dataSize;
        return dataSize + (PACKAGE_REGION_SIZE - rem);
    }
    
    Package::~Package() {
        this->Close();
    }
    
    void Package::WriteFile(std::string filename, uint8_t *content, uint32_t contentLength) {
        if (!this->_writenHeader) {
            this->_writeHeader();
        }
        assert(filename.length() < 96);
        
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        
        uint32_t oldNextFileHeader = header->nextFileHeaderOffset;
        
        // Find the file header slot
        Platform::MemoryMappedRegionPtr fileRegion = this->_file->MapRegion(localOffsetToRegion(header->nextFileHeaderOffset), PACKAGE_REGION_SIZE);
        
        uint32_t fileHeaderOffset = header->nextFileHeaderOffset;
        
        // Write filedata to the file header
        PackageDiskFile* file = fileRegion->Data<PackageDiskFile>(localOffsetToRegionOffset(fileHeaderOffset));
        assert(file->magic == PACKAGE_FILE_MAGIC);
        
        assert(header->nextRegionOffset % PACKAGE_REGION_SIZE == 0);
        uint32_t fileOffset = header->nextRegionOffset;
        
        std::memcpy(&file->name, filename.c_str(), filename.length());
        file->offset = fileOffset;
        file->size = file->decompressedSize = contentLength;
        file->nextFileOffset = header->nextFileHeaderOffset += sizeof(PackageDiskFile);
        
        // Write the file to the next region
        Platform::MemoryMappedRegionPtr contentRegion = this->_file->MapRegion(header->nextRegionOffset, roundSizeToRegionSize(contentLength));
        char* contentData = contentRegion->Data<char>();
        
        std::memcpy(contentData, content, contentLength);
        
        header->nextRegionOffset += roundSizeToRegionSize(contentLength);
        header->numOfFiles++;
        
        // Check to see if we've exceaded the current file header region
        if (oldNextFileHeader > localOffsetToRegion(header->nextFileHeaderOffset)) {
            Platform::MemoryMappedRegionPtr fileChunk = this->_file->MapRegion(header->nextRegionOffset, sizeof(PackageDiskFileChunk));
            
            PackageDiskFileChunk* fileChunkData = fileChunk->Data<PackageDiskFileChunk>();
            assert(fileChunkData != NULL);
            PackageDiskFileChunk fileChunkTemplate;
            std::memcpy(fileChunkData, &fileChunkTemplate, sizeof(fileChunkTemplate));
            
            this->_file->UnmapRegion(fileChunk);
            
            header->nextRegionOffset += PACKAGE_REGION_SIZE;
        }
        
        // Add the offset to the file object to the fast lookup table
        this->_fastFileLookup[filename] = fileHeaderOffset;
    
        // Close regions
        this->_file->UnmapRegion(fileRegion);
        this->_file->UnmapRegion(contentRegion);
    }
    
    uint8_t* Package::ReadFile(std::string filename, uint32_t& contentLength) {
        if (!this->_writenHeader) {
            throw "File not found";
        }
        
        const char* filename_c = filename.c_str();
        uint32_t filename_len = filename.length();
        
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        
        // Check the fast lookup table for the filename
        uint32_t fileHeaderOffset = 0;
        if (this->_fastFileLookup.count(filename) > 0) {
            fileHeaderOffset = this->_fastFileLookup[filename];
        } else {
            uint32_t currentOffset = header->firstFileOffset;
            uint32_t regionOffset = localOffsetToRegion(currentOffset);
            // map the current region into memory
            Platform::MemoryMappedRegionPtr regionPtr = this->_file->MapRegion(regionOffset, PACKAGE_REGION_SIZE);
            for (short i = 0; i < header->numOfFiles; i++) {
                PackageDiskFile* file = regionPtr->Data<PackageDiskFile>(localOffsetToRegionOffset(currentOffset));
                // assert on object magic
                assert(file->magic == PACKAGE_FILE_MAGIC);
                // check to see if we have the correct filename
                if (std::strncat((char*) file->name, filename_c, 96)) {
                    fileHeaderOffset = currentOffset;
                    break;
                }
                
                // if we don't then go to the next file object
                currentOffset = file->nextFileOffset;
                
                // check that the next file object does'nt exist in another region
                if (regionOffset != localOffsetToRegion(currentOffset)) {
                    this->_file->UnmapRegion(regionPtr);
                    regionOffset = localOffsetToRegion(currentOffset);
                    regionPtr = this->_file->MapRegion(regionOffset, PACKAGE_REGION_SIZE);
                }
            }
            this->_file->UnmapRegion(regionPtr);
        }
        
        if (fileHeaderOffset == 0) {
            throw "File not Found";
        }
        
        // map the header
        Platform::MemoryMappedRegionPtr headerPtr = this->_file->MapRegion(localOffsetToRegion(fileHeaderOffset), PACKAGE_REGION_SIZE);
        PackageDiskFile* fileHeader = headerPtr->Data<PackageDiskFile>(localOffsetToRegionOffset(fileHeaderOffset));
        
        // map the region
        Platform::MemoryMappedRegionPtr dataPtr = this->_file->MapRegion(fileHeader->offset, roundSizeToRegionSize(fileHeader->size));
        
        // copy file data into a heap allocated pointer
        uint8_t* fileData = new uint8_t[fileHeader->size];
        contentLength = fileHeader->size;
        
        std::memcpy(fileData, dataPtr->Data<uint8_t>(), fileHeader->size);
        
        // decompress the file if needed
        if (fileHeader->compression == PackageFileCompressionType::NoCompression) {
            assert(fileHeader->size == fileHeader->decompressedSize);
        } else {
            assert(false);
        }
        
        // decrypt the file if needed
        if (fileHeader->encryption == PackageFileEncryptionType::NoEncryption) {
            // no need to do anything to the data
        } else {
            assert(false);
        }
        
        // unmap both regions
        this->_file->UnmapRegion(headerPtr);
        this->_file->UnmapRegion(dataPtr);
        
        return fileData;
    }
    
    Json::Value& Package::GetIndex() {
        return this->_index;
    }
    
    void Package::SaveIndex() {
        if (!this->_writenHeader) {
            this->_writeHeader();
        }
        // TODO: Stringify Index and walk though StringChunks writing the data to see if we have enough space or we need to allocate more
    }
    
    void Package::Defragment() {
        // TODO: Create a new Package and walk through the current one writing only the most recent revision to files
    }
    
    void Package::Close() {
        this->SaveIndex();
        this->_file->UnmapRegion(this->_headerRegion);
        this->_file->Close();
    }
    
    PackagePtr Package::FromFile(std::string filename) {
        assert(Filesystem::HasSetUserDir());
        if (!Filesystem::FileExists(filename)) {
            Filesystem::TouchFile(filename);
        }
        return new Package(Platform::OpenMemoryMappedFile(Filesystem::GetRealPath(filename), Platform::FileMode::Write));
    }
    
    Package::Package(Platform::MemoryMappedFilePtr f) : _file(f) {
        this->_headerRegion = this->_file->MapRegion(0, sizeof(PackageDiskHeader));
        
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        assert(header != NULL);
        this->_writenHeader = (header->magic[0] == 'E' &&
                               header->magic[1] == 'P' &&
                               header->magic[2] == 'K' &&
                               header->magic[3] == 'G');
        
        // load the index
    }
    
    void Package::_writeHeader() {
        // Write PackageDiskHeader
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        PackageDiskHeader headerTemplate;
        std::memcpy(header, &headerTemplate, sizeof(headerTemplate));
        header->thisUUID = Platform::GenerateUUID();
        std::memset(&header->patchUUID, 0, sizeof(header->patchUUID));
        
        // Write Index StringChunk and set data in header
        Platform::MemoryMappedRegionPtr stringChunk = this->_file->MapRegion(0, 1024);
        
        StringChunk* firstIndexChunk = stringChunk->Data<StringChunk>(512);
        StringChunk firstIndexChunkTemplate;
        std::memcpy(firstIndexChunk, &firstIndexChunkTemplate, sizeof(firstIndexChunkTemplate));
        
        header->firstIndexOffset = 512;
        header->nextRegionOffset += PACKAGE_REGION_SIZE;
        
        this->_file->UnmapRegion(stringChunk);
        
        // Write First PackageDiskFile and set data in header
        Platform::MemoryMappedRegionPtr fileChunk = this->_file->MapRegion(header->nextRegionOffset, sizeof(PackageDiskFileChunk));
        
        PackageDiskFileChunk* firstFileChunk = fileChunk->Data<PackageDiskFileChunk>();
        PackageDiskFileChunk firstFileChunkTemplate;
        std::memcpy(firstFileChunk, &firstFileChunkTemplate, sizeof(firstFileChunkTemplate));
        
        header->firstFileOffset = 4096;
        header->nextRegionOffset += PACKAGE_REGION_SIZE;
        header->nextFileHeaderOffset = 4096;
        
        this->_file->UnmapRegion(fileChunk);
        
        this->_writenHeader = true;
    }
}