#pragma once

#include "stdlib.hpp"

#include "TextureLoader.hpp"

namespace Engine {
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
    
    class FontSheet {
    public:
        FontSheet(Json::Value root, std::string basePath);
        
        bool IsValid();
        
        void DrawText(RenderDriver* render, float x, float y, float charHeight, std::string text);
    private:
        Texture* _texture;
        float _baseSize;
        int _charCount;
        std::vector<FontRectangle> _chars;
        
        void _load(Json::Value root, std::string basePath);
    };
    
    namespace FontSheetReader {
        FontSheet* LoadFont(std::string filename);
    }
}