/*
   Filename: FontSheet.hpp
   Purpose:  NeoFont font rendering system

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

#include "stdlib.hpp"

#include "RenderDriver.hpp"
#include "TextureLoader.hpp"
#include "vendor/json/json.h"

namespace Engine {
    class Texture;
    class RenderDriver;
    
    struct FontRectangle {
        float width, x1, y1, x2, y2;
        
        FontRectangle(float width, float x1, float y1, float x2, float y2) {
            this->width = width;
            this->x1 = x1;
            this->y1 = y1;
            this->x2 = x2;
            this->y2 = y2;
        }
    };
    
    struct FontSize {
        int size;
        std::vector<FontRectangle> chars;
    };
    
    typedef FontSize& FontSizeRef;
    
    class FontSheet {
    public:
        FontSheet(Json::Value root, std::string basePath);
        
        bool IsValid();
        
        void DrawText(RenderDriver* render, float x, float y, float charSize, std::string text);
        float MeasureText(float charSize, std::string text);
    private:
        Texture* _texture;
        float _baseSize;
        float _charSpacing = 0.0f;
        int _charCount;
        std::map<int, FontSize> _sizes;
        
        FontSizeRef _getBestSize(int charSize);
        
        void _loadSize(int size, int charCount, Json::Value sizeRoot);
        void _load(Json::Value root, std::string basePath);
    };
    
    namespace FontSheetReader {
        FontSheet* LoadFont(std::string filename);
    }
}