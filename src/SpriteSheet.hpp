#pragma once

#include "common.hpp"

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