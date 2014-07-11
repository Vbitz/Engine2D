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
    
    bool strcmp_s(unsigned char* strA, uint32_t strASize, unsigned char* strB, uint32_t strBSize) {
        // strB is assumed to be a static array of strBSize size so it may contain many null values
        if (strASize > strBSize) return false;
        for (uint32_t i = 0; i < UINT32_MAX; i++) {
            if (strA != strB) return false;
            else if (i > strASize) return false;
            else strA++; strB++;
        }
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
        assert(header != NULL);
        
        uint32_t oldNextFileHeader = header->nextFileHeaderOffset;
        
        // Find the file header slot
        Platform::MemoryMappedRegionPtr fileRegion = this->_file->MapRegion(localOffsetToRegion(header->nextFileHeaderOffset), PACKAGE_REGION_SIZE);
        char* data = fileRegion->Data<char>();
        assert(data != NULL);
        data += localOffsetToRegionOffset(header->nextFileHeaderOffset);
        
        // Write filedata to the file header
        PackageDiskFile* file = (PackageDiskFile*) data;
        assert(file->magic == PACKAGE_FILE_MAGIC);
        
        assert(header->nextRegionOffset % PACKAGE_REGION_SIZE == 0);
        uint32_t fileOffset = header->nextRegionOffset;
        
        std::memcpy(&file->name, filename.c_str(), filename.length());
        file->offset = fileOffset;
        file->size = contentLength;
        file->nextFileOffset = header->nextFileHeaderOffset += sizeof(PackageDiskFile);
        
        // Write the file to the next region
        Platform::MemoryMappedRegionPtr contentRegion = this->_file->MapRegion(header->nextRegionOffset, roundSizeToRegionSize(contentLength));
        char* contentData = contentRegion->Data<char>();
        assert(contentData != NULL);
        
        std::memcpy(contentData, content, contentLength);
        
        header->nextRegionOffset += roundSizeToRegionSize(contentLength);
        
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
        this->_fastFileLookup[filename] = fileOffset;
    
        // Close regions
        this->_file->UnmapRegion(fileRegion);
        this->_file->UnmapRegion(contentRegion);
    }
    
    unsigned char* Package::ReadFile(std::string filename, uint32_t& contentLength) {
        if (!this->_writenHeader) {
            throw "File not found";
        }
        
        unsigned char* filename_c = (unsigned char*) filename.c_str();
        uint32_t filename_len = filename.length();
        
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        assert(header != NULL);
        
        // Check the fast lookup table for the filename
        uint32_t fileDataOffset = 0;
        if (this->_fastFileLookup.count(filename) > 0) {
            fileDataOffset = this->_fastFileLookup[filename];
        } else {
            uint32_t currentOffset = header->firstFileOffset;
            uint32_t regionOffset = localOffsetToRegion(currentOffset);
            // map the current region into memory
            Platform::MemoryMappedRegionPtr regionPtr = this->_file->MapRegion(regionOffset, PACKAGE_REGION_SIZE);
            for (short i = 0; i < header->numOfFiles; i++) {
                char* data = regionPtr->Data<char>();
                data += localOffsetToRegionOffset(currentOffset);
                PackageDiskFile* file = (PackageDiskFile*) data;
                // assert on object magic
                assert(file->magic == PACKAGE_FILE_MAGIC);
                // check to see if we have the correct filename
                if (strcmp_s(filename_c, filename_len, file->name, 96)) {
                    fileDataOffset = currentOffset;
                    break;
                }
                
                // if we don't then go to the next file object
                file++;
                
                // check that the next file object does'nt exist in another region
                
            }
            this->_file->UnmapRegion(regionPtr);
            delete regionPtr;
        }
        
        if (fileDataOffset == 0) {
            throw "File not Found";
        }
        
        // map the region and memcpy the data into a new array
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
        delete this->_headerRegion;
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
    }
    
    void Package::_writeHeader() {
        // Write PackageDiskHeader
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        assert(header != NULL);
        PackageDiskHeader headerTemplate;
        std::memcpy(header, &headerTemplate, sizeof(headerTemplate));
        header->thisUUID = Platform::GenerateUUID();
        std::memset(&header->patchUUID, 0, sizeof(header->patchUUID));
        
        // Write Index StringChunk and set data in header
        Platform::MemoryMappedRegionPtr stringChunk = this->_file->MapRegion(0, 1024);
        
        char* stringChunkBase = stringChunk->Data<char>();
        assert(stringChunkBase != NULL);
        stringChunkBase += 512;
        StringChunk* firstIndexChunk = (StringChunk*) stringChunkBase;
        StringChunk firstIndexChunkTemplate;
        std::memcpy(firstIndexChunk, &firstIndexChunkTemplate, sizeof(firstIndexChunkTemplate));
        
        header->firstIndexOffset = 512;
        header->nextRegionOffset += PACKAGE_REGION_SIZE;
        
        this->_file->UnmapRegion(stringChunk);
        
        // Write First PackageDiskFile and set data in header
        Platform::MemoryMappedRegionPtr fileChunk = this->_file->MapRegion(header->nextRegionOffset, sizeof(PackageDiskFileChunk));
        
        PackageDiskFileChunk* firstFileChunk = fileChunk->Data<PackageDiskFileChunk>();
        assert(firstFileChunk != NULL);
        PackageDiskFileChunk firstFileChunkTemplate;
        std::memcpy(firstFileChunk, &firstFileChunkTemplate, sizeof(firstFileChunkTemplate));
        
        header->firstFileOffset = 4096;
        header->nextRegionOffset += PACKAGE_REGION_SIZE;
        header->nextFileHeaderOffset = 4096;
        
        this->_file->UnmapRegion(fileChunk);
        
        this->_writenHeader = true;
    }
}