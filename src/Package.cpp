#include "Package.hpp"

#include "vendor/zlib123/zlib.h"

namespace Engine {
    // For performance reasons right now Package does not use PHYSFS and insteed uses memory mapped io
    
    Package::~Package() {
        this->Close();
    }
    
    void Package::WriteFile(std::string filename, unsigned char *content, unsigned long contentLength) {
        if (!this->_writenHeader) {
            this->_writeHeader();
        }
    }
    
    unsigned char* Package::ReadFile(std::string filename, unsigned long& contentLength) {
        if (!this->_writenHeader) {
            throw "File not found";
        }
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
        
        header->nextRegionOffset = 4096;
        header->firstIndexOffset = 512;
        
        this->_file->UnmapRegion(stringChunk);
        
        // Write First PackageDiskFile and set data in header
        Platform::MemoryMappedRegionPtr fileChunk = this->_file->MapRegion(4096, sizeof(PackageDiskFileChunk));
        
        PackageDiskFileChunk* firstFileChunk = fileChunk->Data<PackageDiskFileChunk>();
        assert(firstFileChunk != NULL);
        PackageDiskFileChunk firstFileChunkTemplate;
        std::memcpy(firstFileChunk, &firstFileChunkTemplate, sizeof(firstFileChunkTemplate));
        
        header->firstFileOffset = 4096;
        header->nextRegionOffset = 8192;
        
        this->_writenHeader = true;
    }
}