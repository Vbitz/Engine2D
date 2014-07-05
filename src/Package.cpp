#include "Package.hpp"

#include "vendor/zlib123/zlib.h"

namespace Engine {
    // For performance reasons right now Package does not use PHYSFS and insteed uses memory mapped io
    
    void Package::WriteFile(std::string filename, unsigned char *content, unsigned long contentLength) {
        
    }
    
    unsigned char* Package::ReadFile(std::string filename, unsigned long& contentLength) {
        
    }
    
    Json::Value& Package::GetIndex() {
        
    }
    
    void Package::SaveIndex() {
        
    }
    
    PackagePtr Package::FromFile(std::string filename) {
        
    }
    
    Package::Package(Filesystem::File f) {
        
    }
}