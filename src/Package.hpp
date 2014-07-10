#pragma once

#include "vendor/json/json.h"

#include "stdlib.hpp"
#include "Platform.hpp"
#include "Filesystem.hpp"

#define PACKAGE_MAX_SIZE 4294967296
#define PACKAGE_FILES_PER_CHUNK 32
#define PACKAGE_REGION_SIZE 4096
#define PACKAGE_FILE_MAGIC 0xDEADBEEF

namespace Engine {
    
    /*
     Max Package Size 4.2gb
     
     Package Format on Disk
     -------------------------------------------- Start
     PackageDiskHeader - 64 bytes length
     PADDING - 448 bytes length
     StringChunk - Index - 512 bytes length 502 bytes usable
     PADDING - 3072 bytes length
     PackageDiskFile Chunk - 4096 bytes length, 32 files per chunk
     -------------------------------------------- End of Header
     File Content - variable length
     
     More StringChunks or PackageDiskHeader Chunks
     -------------------------------------------- End of File
     */
    
#pragma pack(0)
    struct PackageDiskHeader {
        uint8_t magic[4] = {'E', 'P', 'K', 'G'};
        uint16_t version = 0x0001;
        uint32_t firstFileOffset;
        
        Platform::UUID thisUUID;
        Platform::UUID patchUUID; // filled with 0x00 if this is not a patch file
        
        uint32_t firstIndexOffset;
        
        uint16_t numOfFiles = 0;
        
        // All regions alligned on 0x1000 (4096) bytes
        uint32_t nextRegionOffset = 0;
        uint32_t nextFileHeaderOffset;
    }; // length = 64 bytes
    
    static_assert(sizeof(PackageDiskHeader) == 64, "For alignment the size of PackageDiskHeader must equal 64");
    
    enum class PackageFileCompressionType : uint8_t {
        NoCompression = 0x00,
        DeflateCompression = 0x01
    };
    
    enum class PackageFileEncryptionType : uint8_t {
        NoEncryption = 0x00
    };

#pragma pack(0)
    struct PackageDiskFile {
        uint32_t magic = PACKAGE_FILE_MAGIC;
        
        uint32_t offset = 0;
        uint32_t size = 0;
        
        PackageFileCompressionType compression =
            PackageFileCompressionType::NoCompression;
        PackageFileEncryptionType encryption =
            PackageFileEncryptionType::NoEncryption;
        uint8_t padding1[2] = {0x00, 0x00};
        
        uint32_t latestRevisonOffset = NULL;
        uint32_t nextFileOffset = NULL;
        uint8_t padding2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        unsigned char name[96] = { 0x00 };
    }; // length = 128 bytes
    
    static_assert(sizeof(PackageDiskFile) == 128, "For alignment the size of PackageDiskFile must equal 128");
    
#pragma pack(0)
    struct PackageDiskFileChunk {
        PackageDiskFile files[PACKAGE_FILES_PER_CHUNK];
    };
    
    static_assert(sizeof(PackageDiskFileChunk) == PACKAGE_REGION_SIZE, "For alignment the size of PackageDiskFileChunk must equal the region size");
    
#pragma pack(0)
    struct StringChunk {
        uint32_t magic = 0xCAFEBABE;
        uint32_t nextOffset = 0;
        uint16_t length = 0;
        unsigned char content[512 - (sizeof(nextOffset) + sizeof(length) + sizeof(magic))] = { 0x00 };
    }; // length 512 bytes
        
    static_assert(sizeof(StringChunk) == 512, "");
        
    ENGINE_CLASS(Package);
    
    class Package {
    public:
        ~Package();
        
        void WriteFile(std::string filename, unsigned char* content, unsigned long contentLength);
        unsigned char* ReadFile(std::string filename, unsigned long& contentLength);
        
        Json::Value& GetIndex();
        void SaveIndex();
        
        void Defragment();
        
        void Close();
        
        static PackagePtr FromFile(std::string filename);
    private:
        Package(Platform::MemoryMappedFilePtr f);
        
        void _writeHeader();
        
        Platform::MemoryMappedFilePtr _file;
        
        std::unordered_map<std::string, uint32_t> _fastFileLookup;
        
        bool _writenHeader = false;
        Platform::MemoryMappedRegionPtr _headerRegion;
        
        Json::Value _index = Json::Value(Json::objectValue);
    };
}