#include "Package.hpp"

#include <cstring>

#include "vendor/zlib123/zlib.h"

namespace Engine {
    // For performance reasons right now Package does not use PHYSFS and insteed uses memory mapped io
    
    const PackageFileFlags Package::DefaultFileFlags = PackageFileFlags();
    const PackageFileFlags Package::CompressedFileFlags = PackageFileFlags(PackageFileCompressionType::DeflateCompression);
    
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
    
    bool Package::FileExists(std::string filename) {
        return this->_getFileHeaderOffset(filename) != 0;
    }
    
    void Package::WriteFile(std::string filename, uint8_t *content, uint32_t contentLength, PackageFileFlags flags) {
        if (!this->_writenHeader) {
            this->_writeHeader();
        }
        assert(filename.length() < 96);
        
        uint8_t *compressedContent = content;
        uint32_t compressedContentLength = contentLength;
        
        // Compress the data if we need to
        if (flags.compression == PackageFileCompressionType::DeflateCompression) {
            size_t destLength = compressBound(contentLength);
            uint8_t *dest = new uint8_t[destLength];
            int err = compress(dest, &destLength, compressedContent, compressedContentLength);
            if (err != Z_OK) {
                // compress failed
                throw "Compress Failed";
            }
            compressedContent = dest;
            compressedContentLength = destLength;
        }
        
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
        file->size = compressedContentLength;
        file->decompressedSize = contentLength;
        file->flags = flags;
        
        file->nextFileOffset = header->nextFileHeaderOffset += sizeof(PackageDiskFile);
        
        // Write the file to the next region
        Platform::MemoryMappedRegionPtr contentRegion = this->_file->MapRegion(header->nextRegionOffset, roundSizeToRegionSize(contentLength));
        char* contentData = contentRegion->Data<char>();
        
        std::memcpy(contentData, compressedContent, compressedContentLength);
        
        header->nextRegionOffset += roundSizeToRegionSize(compressedContentLength);
        header->numOfFiles++;
        
        // Check to see if we've exceaded the current file header region
        if (localOffsetToRegion(oldNextFileHeader) != localOffsetToRegion(header->nextFileHeaderOffset)) {
            Platform::MemoryMappedRegionPtr fileChunk = this->_file->MapRegion(header->nextRegionOffset, sizeof(PackageDiskFileChunk));
            
            PackageDiskFileChunk* fileChunkData = fileChunk->Data<PackageDiskFileChunk>();
            assert(fileChunkData != NULL);
            PackageDiskFileChunk fileChunkTemplate;
            std::memcpy(fileChunkData, &fileChunkTemplate, sizeof(fileChunkTemplate));
            
            this->_file->UnmapRegion(fileChunk);
            
            file->nextFileOffset = header->nextRegionOffset;
            header->nextFileHeaderOffset = header->nextRegionOffset;
            
            header->nextRegionOffset += PACKAGE_REGION_SIZE;
        }
        
        // Add the offset to the file object to the fast lookup table
        this->_fastFileLookup[filename] = fileHeaderOffset;
    
        // Close regions
        this->_file->UnmapRegion(fileRegion);
        this->_file->UnmapRegion(contentRegion);
        
        // Free Compressed Data
        if (flags.compression != PackageFileCompressionType::NoCompression) {
            delete [] compressedContent;
        }
    }
    
    uint8_t* Package::ReadFile(std::string filename, uint32_t& contentLength) {
        uint32_t fileHeaderOffset = this->_getFileHeaderOffset(filename);
        
        if (fileHeaderOffset == 0) {
            throw "File Not found";
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
        if (fileHeader->flags.compression == PackageFileCompressionType::NoCompression) {
            assert(fileHeader->size == fileHeader->decompressedSize);
        } else {
            size_t decompressedFileSize = fileHeader->decompressedSize;
            uint8_t* decompressedFileData = new uint8_t[decompressedFileSize];
            int err = uncompress(decompressedFileData, &decompressedFileSize, fileData, fileHeader->size);
            assert(decompressedFileSize == fileHeader->decompressedSize);
            delete [] fileData;
            contentLength = decompressedFileSize;
            fileData = decompressedFileData;
        }
        
        // decrypt the file if needed
        if (fileHeader->flags.encryption == PackageFileEncryptionType::NoEncryption) {
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
        
        if (this->_savedIndex) {
            return;
        }
        
        std::stringstream ss;
        
        ss << this->_index;
        
        std::string indexContent = ss.str();
        
        this->WriteFile(INDEX_FILENAME, (uint8_t*) indexContent.c_str(), indexContent.length(), DefaultFileFlags);
        
        this->_savedIndex = true;
    }
    
    void Package::Defragment() {
        // TODO: Create a new Package and walk through the current one writing only the most recent revision to files
    }
    
    void Package::Close() {
        // Don't implictly save indexes until we can be sure it has'nt changed or we can overwrite without changing block layout
        // this->SaveIndex();
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
    
    PackagePtr Package::FromJsonSpec(std::string inputFile, std::string outputFile) {
        return Package::FromJsonSpec(Filesystem::LoadJsonFile(inputFile), outputFile);
    }
    
    PackagePtr Package::FromJsonSpec(Json::Value inputFile, std::string outputFile) {
        assert(inputFile.isObject());
        Json::Value fileList = inputFile["files"];
        assert(fileList.isArray());
        
        PackagePtr ret = Package::FromFile(outputFile);
        
        for (auto iter = fileList.begin(); iter != fileList.end(); iter++) {
            Json::Value value = *iter;
            
            std::string srcFilename, destFilename;
            bool compress = true;
            
            if (value.isString()) {
                assert(value.isString());
                srcFilename = destFilename = value.asString();
            } else {
                assert(value.isObject());
                srcFilename = value["src"].asString();
                destFilename = value.get("dest", srcFilename).asString();
                compress = value.get("compress", true).asBool();
            }
            long fileLength = 0;
            char* fileContent = Filesystem::GetFileContent(srcFilename, fileLength);
            assert(fileLength < UINT32_MAX);
            ret->WriteFile(destFilename, (uint8_t*) fileContent, (uint32_t) fileLength, compress ? CompressedFileFlags : DefaultFileFlags);
        }
        
        if (!inputFile.get("index", Json::nullValue).isNull()) {
            ret->GetIndex() = inputFile["index"];
            ret->SaveIndex();
        }
        
        return ret;
    }
    
    Package::Package(Platform::MemoryMappedFilePtr f) : _file(f) {
        this->_headerRegion = this->_file->MapRegion(0, sizeof(PackageDiskHeader));
        
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        assert(header != NULL);
        this->_writenHeader = (header->magic[0] == 'E' &&
                               header->magic[1] == 'P' &&
                               header->magic[2] == 'K' &&
                               header->magic[3] == 'G');
        
        if (this->FileExists(INDEX_FILENAME)) {
            uint32_t indexLength = 0;
            uint8_t* indexContent = this->ReadFile(INDEX_FILENAME, indexLength);
            Json::Reader reader;
            reader.parse(std::string((char*) indexContent, indexLength), this->_index);
        }
    }
    
    void Package::_writeHeader() {
        // Write PackageDiskHeader
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        PackageDiskHeader headerTemplate;
        std::memcpy(header, &headerTemplate, sizeof(headerTemplate));
        header->thisUUID = Platform::GenerateUUID();
        std::memset(&header->patchUUID, 0, sizeof(header->patchUUID));
        
        header->nextRegionOffset = PACKAGE_REGION_SIZE;
        
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
    
    uint32_t Package::_getFileHeaderOffset(std::string filename) {
        if (this->_fastFileLookup.count(filename) != 0) {
            return this->_fastFileLookup[filename];
        }
        
        if (!this->_writenHeader) {
            return 0;
        }
        
        const char* filename_c = filename.c_str();
        
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
                if (std::strncmp((char*) file->name, filename_c, 96) == 0) {
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
        
        if (fileHeaderOffset != 0) {
            this->_fastFileLookup[filename] = fileHeaderOffset;
        }
        
        return fileHeaderOffset;
    }
}
