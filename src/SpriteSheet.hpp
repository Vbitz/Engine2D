/*
   Filename: SpriteSheet.hpp
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

#pragma once

#include "TextureLoader.hpp"

namespace Engine {
    class Texture;
    
    struct SpriteLocation {
        float x, y, w, h;
    };
    
    struct SpriteAnimation {
        std::vector<SpriteLocation> locations;
        float delay;
    };
    
    struct SpriteAnimationStatus {
        SpriteAnimation* animation;
        int currentIndex;
        double currentTime;
    };
    
    struct Sprite {
        Texture* tex;
        SpriteLocation loc;
    };
    
    class SpriteSheet {
    public:
        SpriteSheet();
        SpriteSheet(Json::Value root, std::string basePath);
        ~SpriteSheet();
        
        bool IsValid();
        
        Sprite GetSprite(std::string index);
        
        void ResetSpriteAnimation(std::string index);
        
    private:
        void _load(Json::Value root, std::string basePath);
        int _isAnimation(std::string index);
        void _createAnimationStatus(std::string index);
        SpriteLocation _get(std::string index);
        
        std::map<std::string, SpriteLocation> _locations;
        std::map<std::string, SpriteAnimation> _animations;
        std::map<std::string, SpriteAnimationStatus> _animationStatus;
        
        Texture* _texture = NULL;
    };
    
    namespace SpriteSheetReader {
        SpriteSheet* LoadSpriteSheetFromFile(std::string filename);
    }
}