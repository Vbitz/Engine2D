#pragma once

#include "stdlib.hpp"
#include "Platform.hpp"

namespace Engine {
    struct PackageDiskHeader {
        unsigned char magic[4] = {'E', 'P', 'K', 'G'};
        unsigned short version = 0x0001;
        unsigned long firstFileOffset;
        
        Platform::UUID thisUUID;
        Platform::UUID patchUUID; // filled with 0x00 if this is not a patch file
        
        unsigned long stringTableOffset;
        unsigned long stringTableSize;
        
        unsigned long indexOffset;
        unsigned long indexSize;
        
        unsigned long signatureOffset = NULL;
        unsigned long signatureSize = NULL;
    }; // length = 94 bytes
    
    enum class PackageFileCompressionType : unsigned char {
        NoCompression = 0x00,
        DeflateCompression = 0x01
    };
    
    struct PackageDiskFile {
        Platform::UUID uuid;
        unsigned long nameOffset;
        
        unsigned long offset;
        unsigned long size;
        
        PackageFileCompressionType compression;
        unsigned char padding[3] = {0x00, 0x00, 0x00};
        unsigned long nextFileOffset = NULL;
    }; // length = 52 bytes
    
    ENGINE_CLASS(Package);
    
    class Package {
    public:
        Package();
        
        void Save(std::string filename);
        
        static PackagePtr FromFile(std::string filename);
    private:
        
        
    };
}