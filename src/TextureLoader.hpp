#pragma once

#include "common.hpp"

#include "ResourceManager.hpp"

namespace Engine {
    
    namespace ResourceManager {
        class ImageResource;
    }
    
    class Texture {
    public:
        Texture();
        Texture(GLuint textureID);
        ~Texture();
        
        void Invalidate();
        
        void Save(std::string filename);
        
        bool IsValid();
        
        void Begin();
        void End();
        
        int GetWidth();
        int GetHeight();
        
    private:
        void _setTextureID(GLuint textureID);
        void _setTextureID(GLuint textureID, bool deleteOld);
        GLuint _textureID = UINT_MAX;
        int _width, _height;
    };
    
    namespace ImageReader {
        Texture* TextureFromFileBuffer(unsigned char* texture, long bufferLength);
        
        Texture* TextureFromBuffer(unsigned char* texture, int width, int height);
        Texture* TextureFromBuffer(GLuint textureID, unsigned char* texture, int width, int height);

        Texture* TextureFromBuffer(float* texture, int width, int height);
        Texture* TextureFromBuffer(GLuint textureID, float* texture, int width, int height);
        
        ResourceManager::ImageResource* TextureFromFile(std::string filename);
    }
    
    namespace ImageWriter {
        bool SaveBufferToFile(std::string filename, unsigned char* pixels, int width, int height);
        bool SaveBufferToFile(std::string filename, unsigned char* pixels, int width, int height, bool topDown);
    }
}