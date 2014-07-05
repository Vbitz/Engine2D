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
     StringChunk - String Table - 512 bytes length 506 bytes usable
     StringChunk - Index - 512 bytes length 506 bytes usable
     PackageDiskFile Chunk - 1024 bytes length, 32 files per chunk
     File Content - variable length
     
     More StringChunks or PackageDiskHeader Chunks
     -------------------------------------------- End of File
     */
    
    struct PackageDiskHeader {
        unsigned char magic[4] = {'E', 'P', 'K', 'G'};
        unsigned short version = 0x0001;
        unsigned int firstFileOffset;
        
        Platform::UUID thisUUID;
        Platform::UUID patchUUID; // filled with 0x00 if this is not a patch file
        
        unsigned int stringTableFirstOffset;
        unsigned int indexFirstOffset;
        
        unsigned int signatureOffset = NULL;
        unsigned int signatureSize = NULL;
        unsigned char padding[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }; // length = 64 bytes
    
    enum class PackageFileCompressionType : unsigned char {
        NoCompression = 0x00,
        DeflateCompression = 0x01
    };
    
    struct PackageDiskFile {
        unsigned int nameOffset;
        
        unsigned int offset;
        unsigned int size;
        
        PackageFileCompressionType compression;
        unsigned char padding1[3] = {0x00, 0x00, 0x00};
        unsigned int nextFileOffset = NULL;
        unsigned char padding2[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }; // length = 32 bytes
    
    struct StringChunk {
        unsigned int nextOffset;
        unsigned short length;
        unsigned char content[512 - (sizeof(nextOffset) + sizeof(length))];
    }; // length 512 bytes
        
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