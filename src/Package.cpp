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
        if (!Filesystem::FileExists(filename)) {
            Filesystem::TouchFile(filename);
        }
        return new Package(Platform::OpenMemoryMappedFile(Filesystem::GetRealPath(filename), Platform::FileMode::Write));
    }
    
    Package::Package(Platform::MemoryMappedFilePtr f) : _file(f) {
        this->_headerRegion = this->_file->MapRegion(0, sizeof(PackageDiskHeader));
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        this->_writenHeader = (header->magic[0] == 'E' &&
                               header->magic[1] == 'P' &&
                               header->magic[2] == 'K' &&
                               header->magic[3] == 'G');
    }
    
    void Package::_writeHeader() {
        PackageDiskHeader* header = this->_headerRegion->Data<PackageDiskHeader>();
        assert(header != NULL);
        PackageDiskHeader hdr;
        std::memcpy(header, &hdr, sizeof(hdr));
        header->thisUUID = header->patchUUID = Platform::GenerateUUID();
        this->_writenHeader = true;
    }
}