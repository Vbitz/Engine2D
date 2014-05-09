/*
   Filename: TextureLoader.hpp
   Purpose:  Texture loader

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include "ResourceManager.hpp"

namespace Engine {
    
    namespace ResourceManager {
        class ImageResource;
    }
    
    class Texture {
    public:
        Texture();
        Texture(RenderGL3* render, GLuint textureID);
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
        
        RenderGL3* _render;
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