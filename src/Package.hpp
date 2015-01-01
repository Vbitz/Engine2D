#pragma once

#include "vendor/json/json.h"

#include "stdlib.hpp"
#include "Platform.hpp"
#include "Filesystem.hpp"

#define PACKAGE_MAX_SIZE 4294967295
#define PACKAGE_FILES_PER_CHUNK 32
#define PACKAGE_REGION_SIZE 4096
#define PACKAGE_FILE_MAGIC 0xDEADBEEF
#define PACKAGE_FILE_VERSION 0x0003

namespace Engine {
    
    /*
     Max Package Size 4.2gb
     
     Package Format on Disk
     -------------------------------------------- Start
     PackageDiskHeader - 64 bytes length
     PADDING - 4032 bytes length
     PackageDiskFile Chunk - 4096 bytes length, 32 files per chunk
     -------------------------------------------- End of Header
     File Content - variable length
     
     More StringChunks or PackageDiskHeader Chunks
     -------------------------------------------- End of File
     */

#pragma pack(push, 1)
    struct PackageDiskHeader {
        uint8_t magic[4];
        uint16_t version = PACKAGE_FILE_VERSION;
        uint32_t firstFileOffset;
        
        Platform::UUID thisUUID;
        Platform::UUID patchUUID; // filled with 0x00 if this is not a patch file
        
        uint32_t firstIndexOffset;
        
        uint16_t numOfFiles = 0;
        
        // All regions alligned on 0x1000 (4096) bytes
        uint32_t nextRegionOffset = 0;
        uint32_t nextFileHeaderOffset;

		uint64_t padding1 = 0;
    }; // length = 64 bytes
#pragma pack(pop)
    
    static_assert(sizeof(PackageDiskHeader) == 64, "For alignment the size of PackageDiskHeader must equal 64");
    
    enum class PackageFileCompressionType : uint8_t {
        NoCompression = 0x00,
        DeflateCompression = 0x01
    };
    
    enum class PackageFileEncryptionType : uint8_t {
        NoEncryption = 0x00
    };

#pragma pack(push, 1)
    struct PackageFileFlags {
		PackageFileCompressionType compression = PackageFileCompressionType::NoCompression;
        PackageFileEncryptionType encryption = PackageFileEncryptionType::NoEncryption;
		uint8_t padding1[2];
        uint8_t padding2[4];
        
        PackageFileFlags() {
			memset(&padding1, NULL, sizeof(padding1) + sizeof(padding2));
		}
        PackageFileFlags(PackageFileCompressionType c) : compression(c) {
			memset(&padding1, NULL, sizeof(padding1) + sizeof(padding2));
		}
	};
#pragma pack(pop)
    
    static_assert(sizeof(PackageFileFlags) == 8, "For alignment the size of PackageFileFlags must equal 8");

#pragma pack(push, 1)
    struct PackageDiskFile {
        uint32_t magic = PACKAGE_FILE_MAGIC;
        
        uint32_t offset = 0;
        uint32_t size = 0;
        uint32_t decompressedSize = 0;
        
        PackageFileFlags flags;
        
        uint32_t latestRevisonOffset = 0;
        uint32_t nextFileOffset = 0;
        
        unsigned char name[96];

		PackageDiskFile() {
			memset(&name, NULL, sizeof(name));
		}
    }; // length = 128 bytes
#pragma pack(pop)
    
    static_assert(sizeof(PackageDiskFile) == 128, "For alignment the size of PackageDiskFile must equal 128");

#pragma pack(push, 1)
    struct PackageDiskFileChunk {
        PackageDiskFile files[PACKAGE_FILES_PER_CHUNK];
	};
#pragma pack(pop)
    
    static_assert(sizeof(PackageDiskFileChunk) == PACKAGE_REGION_SIZE, "For alignment the size of PackageDiskFileChunk must equal the region size");
    
#pragma pack(push, 1)
    struct StringChunk {
        uint32_t magic = 0xCAFEBABE;
        uint32_t nextOffset = 0;
        uint16_t length = 0;
		unsigned char content[512 - (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t))];

		StringChunk() {
			memset(&content, NULL, sizeof(content));
		}
    }; // length 512 bytes
#pragma pack(pop)
        
    static_assert(sizeof(StringChunk) == 512, "Bad StringChunk Size");
        
    ENGINE_CLASS(Package);
    
    class Package {
    public:
        ~Package();
        
        bool FileExists(std::string filename);
        void WriteFile(std::string filename, uint8_t* content, uint32_t contentLength, PackageFileFlags flags);
        uint8_t* ReadFile(std::string filename, uint32_t& contentLength);
        
        Json::Value& GetIndex();
        void SaveIndex();
        
        void Defragment();
        
        void Close();
        
        const char* INDEX_FILENAME = "__INDEX__";
        
        static PackagePtr FromFile(std::string filename);
        static PackagePtr FromJsonSpec(std::string inputFile, std::string outputFile);
        static PackagePtr FromJsonSpec(Json::Value inputFile, std::string outputFile);
        
        static const PackageFileFlags DefaultFileFlags;
        static const PackageFileFlags CompressedFileFlags;
    private:
        Package(Platform::MemoryMappedFilePtr f);
        
        void _writeHeader();
        uint32_t _getFileHeaderOffset(std::string filename);
        
        Platform::MemoryMappedFilePtr _file;
        
        std::unordered_map<std::string, uint32_t> _fastFileLookup;
        
        bool _writenHeader = false;
        bool _savedIndex = false;
        Platform::MemoryMappedRegionPtr _headerRegion;
        
        Json::Value _index = Json::Value(Json::objectValue);
    };
}