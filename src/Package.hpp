#pragma once

#include "vendor/json/json.h"

#include "stdlib.hpp"
#include "Platform.hpp"
#include "Filesystem.hpp"

#define PACKAGE_MAX_SIZE 4294967296
#define PACKAGE_FILES_PER_CHUNK 32

namespace Engine {
    
    /*
     Max Package Size 4.2gb
     
     Package Format on Disk
     -------------------------------------------- Start
     PackageDiskHeader - 64 bytes length
     -------------------------------------------- End of Header
     StringChunk - Index - 512 bytes length 506 bytes usable
     PackageDiskFile Chunk - 1024 bytes length, 8 files per chunk
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
        
        uint32_t indexFirstOffset;
        
        uint8_t padding[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
        unsigned char name[96];
        
        uint32_t offset;
        uint32_t size;
        
        PackageFileCompressionType compression;
        PackageFileEncryptionType encryption;
        uint8_t padding1[2] = {0x00, 0x00};
        uint32_t nextFileOffset = NULL;
        uint8_t padding2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }; // length = 92 bytes
    
    static_assert(sizeof(PackageDiskFile) == 128, "For alignment the size of PackageDiskFile must equal 128");
    
#pragma pack(0)
    struct PackageDiskFileChunk {
        PackageDiskFile files[8];
    };
    
    static_assert(sizeof(PackageDiskFileChunk) == 1024, "For alignment the size of PackageDiskFileChunk must equal 1024");
    
#pragma pack(0)
    struct StringChunk {
        uint32_t nextOffset;
        uint16_t length;
        unsigned char content[512 - (sizeof(nextOffset) + sizeof(length))];
    }; // length 512 bytes
        
    static_assert(sizeof(StringChunk) == 512, "");
        
    ENGINE_CLASS(Package);
    
    class Package {
    public:
        void WriteFile(std::string filename, unsigned char* content, unsigned long contentLength);
        unsigned char* ReadFile(std::string filename, unsigned long& contentLength);
        
        Json::Value& GetIndex();
        void SaveIndex();
        
        static PackagePtr FromFile(std::string filename);
    private:
        Package(Filesystem::File f);
        
        Json::Value _index = Json::Value(Json::objectValue);
    };
        
    class PackageFile {
            
    };
}