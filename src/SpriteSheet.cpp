/*
   Filename: SpriteSheet.cpp
   Purpose:  SpriteSheet loading and animation

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

#include "SpriteSheet.hpp"

#include "FramePerfMonitor.hpp"

namespace Engine {
    
    std::string spriteResolvePath(std::string basePath, std::string path) {
        if (path.find_first_of('/') == 0)
            return path;
        else
            return basePath + path;
    }
    
    SpriteLocation loadLocation(Json::Value value) {
        SpriteLocation ret;
        ret.x = value[0].asFloat();
        ret.y = value[1].asFloat();
        ret.w = value[2].asFloat();
        ret.h = value[3].asFloat();
        return ret;
    }
    
    SpriteSheet::SpriteSheet() {
        
    }
    
    SpriteSheet::SpriteSheet(Json::Value root, std::string basePath) {
        this->_load(root, basePath);
    }
    
    SpriteSheet::~SpriteSheet() {
        
    }
    
    bool SpriteSheet::IsValid() {
        return this != NULL && this->_texture->IsValid();
    }
    
    Sprite SpriteSheet::GetSprite(std::string index) {
        if (this->_isAnimation(index) > 0) {
            double dt = FramePerfMonitor::GetFrameTime();
            if (this->_animationStatus.count(index) == 0) {
                this->_createAnimationStatus(index);
            }
            
            SpriteAnimationStatus* status = &this->_animationStatus[index];
            status->currentTime += dt;
            if (status->currentTime > status->animation->delay) {
                status->currentTime = 0.0f;
                status->currentIndex++;
                if (status->currentIndex
                    > status->animation->locations.size() - 1) {
                    status->currentIndex = 0;
                }
            }
            Sprite ret;
            ret.tex = this->_texture;
            ret.loc = status->animation->locations[status->currentIndex];
            return ret;
        } else {
            Sprite ret;
            ret.tex = this->_texture;
            ret.loc = this->_locations[index];
            return ret;
        }
    }
    
    void SpriteSheet::ResetSpriteAnimation(std::string index) {
        if (this->_animationStatus.count(index) == 0) {
            this->_createAnimationStatus(index);
        } else {
            this->_animationStatus[index].currentTime = 0.0f;
            this->_animationStatus[index].currentIndex = 0;
        }
    }
    
    void SpriteSheet::_load(Json::Value root, std::string basePath) {
        this->_texture = ImageReader::TextureFromFile(spriteResolvePath(basePath, root["texture"].asString()))->GetTexture();
        
        Json::Value sprites = root["sprites"];
        for (auto iter = sprites.begin(); iter != sprites.end(); iter++) {
            std::string key = iter.key().asString();
            this->_locations[key] = loadLocation(sprites[key]);
        }
        
        Json::Value animations = root["animations"];
        for (auto iter = animations.begin(); iter != animations.end(); iter++) {
            std::string key = iter.key().asString();
            Json::Value ani = animations[key];
            SpriteAnimation animation;
            
            animation.delay = ani.get("speed", 0.10).asFloat();
            
            Json::Value images = ani["images"];
            for (auto iter2 = images.begin(); iter2 != images.end(); iter2++) {
                animation.locations.push_back(this->_get(images[iter2.key().asInt()].asString()));
            }
            
            this->_animations[key] = animation;
        }
    }
    
    int SpriteSheet::_isAnimation(std::string index) {
        if (this->_locations.count(index) > 0) {
            return 0;
        } else if (this->_animations.count(index) > 0) {
            return 1;
        } else {
            return -1;
        }
    }
    
    void SpriteSheet::_createAnimationStatus(std::string index) {
        SpriteAnimationStatus status;
        
        status.animation = &this->_animations[index];
        status.currentTime = 0.0f;
        status.currentIndex = 0;
        
        this->_animationStatus[index] = status;
    }
    
    SpriteLocation SpriteSheet::_get(std::string index) {
        return this->_locations[index];
    }
    
    namespace SpriteSheetReader {
        std::string getBasePath(std::string path) {
            return path.substr(0, path.find_last_of('/') + 1);
        }
        
        SpriteSheetPtr LoadSpriteSheetFromFile(std::string filename) {
            std::string fileContent = std::string(Filesystem::GetFileContent(filename));
            
            Json::Reader reader;
            Json::Value root;
            
            reader.parse(fileContent, root);
            
            return new SpriteSheet(root, getBasePath(filename));
        }
    }
}