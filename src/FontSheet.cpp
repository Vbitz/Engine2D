#include "FontSheet.hpp"

#include "Filesystem.hpp"
#include "Draw2D.hpp"

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
    
    void FontSheet::DrawText(RenderDriver* render, float x, float y, float charHeight, std::string text) {
        render->EnableTexture(this->_texture);
        render->BeginRendering(GL_TRIANGLES);
        float currentX = x;
        float chrWidth = charHeight / this->_baseSize;
        float chrHeight = charHeight;
        for (int i = 0; i < text.length(); i++) {
            FontRectangle rect = this->_chars[text[i]];
            render->AddVert(currentX, y, 0.0f, rect.x1, rect.y1);
            render->AddVert(currentX + (chrWidth * rect.width), y, 0.0f, rect.x2, rect.y1);
            render->AddVert(currentX + (chrWidth * rect.width), y + chrHeight, 0.0f, rect.x2, rect.y2);
            render->AddVert(currentX, y, 0.0f, rect.x1, rect.y1);
            render->AddVert(currentX, y + chrHeight, 0.0f, rect.x1, rect.y2);
            render->AddVert(currentX + (chrWidth * rect.width), y + chrHeight, 0.0f, rect.x2, rect.y2);
            currentX += (chrWidth * rect.width);
        }
        
        render->EndRendering();
        render->DisableTexture();
    }
    
    void FontSheet::_load(Json::Value root, std::string basePath) {
        this->_texture = ImageReader::TextureFromFile(fontResolvePath(basePath, root["texture"].asString()))->GetTexture();
        this->_baseSize = root["baseSize"].asFloat();
        this->_charCount = root["charactorCount"].asInt();
        
        Json::Value chars = root["charactors"];
        
        for (int i = 0; i < this->_charCount; i++) {
            Json::Value charRectangle = chars[i];
            this->_chars.push_back(FontRectangle(
                charRectangle["width"].asFloat(),
                charRectangle["x1"].asFloat(),
                charRectangle["y1"].asFloat(),
                charRectangle["x2"].asFloat(),
                charRectangle["y2"].asFloat()
            ));
        }
    }
    
    namespace FontSheetReader {
        FontSheet* LoadFont(std::string filename) {
            std::string fileContent = std::string(Filesystem::GetFileContent(filename));
            
            Json::Reader reader;
            Json::Value root;
            
            reader.parse(fileContent, root);
            
            return new FontSheet(root, filename.substr(0, filename.find_last_of('/') + 1));
        }
    }
}