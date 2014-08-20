/*
   Filename: FontSheet.cpp
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

#include "FontSheet.hpp"

#include "Filesystem.hpp"
#include "RenderDriver.hpp"

#define GLEW_STATIC
#include "vendor/GL/glew.h"

namespace Engine {
    std::string fontResolvePath(std::string basePath, std::string path) {
        if (path.find_first_of('/') == 0)
            return path;
        else
            return basePath + path;
    }
    
    FontSheet::FontSheet(Json::Value root, std::string basePath) {
        this->_load(root, basePath);
    }
    
    bool FontSheet::IsValid() {
        return this->_texture->IsValid();
    }
    
    void FontSheet::DrawText(RenderDriverPtr render, float x, float y, float charSize, std::string text) {
        if (!this->IsValid()) {
            Logger::begin("FontSheet", Logger::LogLevel_Verbose) << "FontSheet Texture reloaded" << Logger::end();
            this->_texture = ImageReader::TextureFromFile(this->_texturePath)->GetTexture();
        }
        render->EnableTexture(this->_texture);
        render->BeginRendering(PolygonMode::Triangles); // I would rather render quads but OGL 3.x does'nt support them
        FontSizeRef size = this->_getBestSize(charSize);
        float currentX = x;
        float chrWidth = charSize / size.size;
        float chrHeight = charSize;
        for (int i = 0; i < text.length(); i++) {
            FontRectangle rect = size.chars[text[i]];
            render->AddVert(currentX, y, 0.0f, rect.x1, rect.y1);
            render->AddVert(currentX + (chrWidth * rect.width), y, 0.0f, rect.x2, rect.y1);
            render->AddVert(currentX + (chrWidth * rect.width), y + chrHeight, 0.0f, rect.x2, rect.y2);
            render->AddVert(currentX, y, 0.0f, rect.x1, rect.y1);
            render->AddVert(currentX, y + chrHeight, 0.0f, rect.x1, rect.y2);
            render->AddVert(currentX + (chrWidth * rect.width), y + chrHeight, 0.0f, rect.x2, rect.y2);
            currentX += (chrWidth * rect.width) + this->_charSpacing;
        }
        render->EndRendering();
        render->DisableTexture();
    }

    float FontSheet::MeasureText(float charSize, std::string text) {
        FontSizeRef size = this->_getBestSize(charSize);
        float textWidth = 0.0f;
        float chrWidth = charSize / size.size;
        for (int i = 0; i < text.length(); i++) {
            textWidth += (chrWidth * size.chars[text[i]].width) + this->_charSpacing;
        }
        return textWidth;
    }
    
    FontSizeRef FontSheet::_getBestSize(int charSize) {
        // If we find a exact size match then use that
        if (this->_sizes.count(charSize)) return this->_sizes[charSize];
        
        // If we can't find that then look for 1 that's double or half the size
        if (this->_sizes.count(charSize * 2)) return this->_sizes[charSize * 2];
        if (this->_sizes.count(charSize / 2)) return this->_sizes[charSize / 2];
        
        // Otherwise find the closest match
        for (auto iter = this->_sizes.begin(); iter != this->_sizes.end(); iter++) {
            if (iter->first > charSize) {
                int upperCharSize = iter->first;
                if (iter != this->_sizes.begin()) {
                    int lowerCharSize = (iter--)->first;
                    int upperDiff = upperCharSize - charSize;
                    int lowerDiff = charSize - lowerCharSize;
                    if (upperDiff > lowerDiff) {
                        return this->_sizes[lowerCharSize];
                    } else {
                        return this->_sizes[upperCharSize];
                    }
                } else {
                    return this->_sizes[upperCharSize];
                }
            }
        }
        
        // Otherwise return the first
        for (auto iter : this->_sizes) {
            return this->_sizes[iter.first];
        }
        
        // Unreachable
        assert(false);
    }
    
    void FontSheet::_loadSize(int size, int charCount, Json::Value sizeRoot) {
        FontSize s;
        s.size = size;
        
        for (int i = 0; i < this->_charCount; i++) {
            Json::Value charRectangle = sizeRoot[i];
            s.chars.push_back(FontRectangle(
                charRectangle["width"].asFloat(),
                charRectangle["x1"].asFloat(),
                charRectangle["y1"].asFloat(),
                charRectangle["x2"].asFloat(),
                charRectangle["y2"].asFloat()
            ));
        }
        
        this->_sizes[size] = s;
    }
    
    void FontSheet::_load(Json::Value root, std::string basePath) {
        this->_texturePath = fontResolvePath(basePath, root["texture"].asString());
        this->_texture = ImageReader::TextureFromFile(this->_texturePath)->GetTexture();
        this->_baseSize = root["baseSize"].asFloat();
        this->_charCount = root["charactorCount"].asInt();
        this->_charSpacing = root.get("charactorSpacing", 0.0f).asFloat();
        
        Json::Value sizes = root["sizes"];
        
        for (auto iter = sizes.begin(); iter != sizes.end(); iter++) {
            std::string keyStrValue = iter.key().asString();
            this->_loadSize(std::stoi(keyStrValue), this->_charCount, *iter);
        }
    }
    
    namespace FontSheetReader {
        FontSheetPtr LoadFont(std::string filename) {
            std::string fileContent = std::string(Filesystem::GetFileContent(filename));
            
            Json::Reader reader;
            Json::Value root;
            
            reader.parse(fileContent, root);
            
            return new FontSheet(root, filename.substr(0, filename.find_last_of('/') + 1));
        }
    }
}