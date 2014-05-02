/*
   Filename: ResourceManager.hpp
   Purpose:  Resource management and loading interface

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

#include "Filesystem.hpp"

#include "vendor/GLFT_Font.hpp"

#include "TextureLoader.hpp"

#include <FreeImage.h>

/*
 Think of Resources like this, If you could put it on the filesystem then it's a Source otherwise it's a Resource
 */
 
namespace Engine {
    class Texture;
    
    namespace ResourceManager {
        class Source {
        public:
            virtual void Unload() {
                if (this->_savedData != NULL) {
                    std::free(this->_savedData);
                }
            }
            
            virtual bool NeedsUpdate();
            
            unsigned char* GetData(long& len);
            
            virtual std::string GetName();
            
        protected:
            virtual unsigned char* _getData(long& len);
            
            unsigned char* _savedData;
            long _savedDataLength;
        };
        
        class DummySource : public Source {
        public:
            bool NeedsUpdate() {
                return false;
            }
            
            std::string GetName() {
                return "DummySource";
            }
            
        protected:
            unsigned char* _getData(long& len) {
                len = 0;
                return new unsigned char[0];
            }
        };
        
        class FileSource : public Source {
        public:
            FileSource(std::string path);
            
            void Unload() override {
                if (this->_lastModify != -1 && this->_savedData != NULL) {
                    std::free(this->_savedData);
                }
            };

            bool NeedsUpdate() override;
            std::string GetName() override;
            
        protected:
            unsigned char* _getData(long& len) override;
            
        private:
            std::string _path;
            long _lastModify;
        };
        
        class WebSource : public Source {
        public:
            WebSource(std::string webRoot, std::string path);

            bool NeedsUpdate() override;
            std::string GetName() override;
            
        protected:
            unsigned char* _getData(long& len) override;
            
        private:
            std::string _webRoot, _path;
        };
        
        class RawSource : public Source {
        public:
            RawSource(const unsigned char* data, long fileLength);
            
            void Unload() {
                // we don't want to free a rawsource
            }
            
            bool NeedsUpdate() {
                return false;
            }
            
            std::string GetName() {
                return "raw:///";
            }
            
        protected:
            unsigned char* _getData(long& len) override;
        
        private:
            unsigned char* data;
            long fileLength;
        };
        
        class Resource {
        public:
            Resource();
            Resource(std::string sourceID);
            ~Resource() {
                this->Unload();
            }
            
            virtual bool IsCritical();
            
            virtual std::string GetName();
            
            void Load();
            void Unload();
            
            void CheckForUpdates(bool force);
            
            bool IsLoaded();
            
        protected:
            void _forceLoad();
            
            virtual void _load();
            virtual void _unload();
            
            Source* _source;
            
            bool _loaded;
        };
        
        class ScriptResource : public Resource {
        public:
            ScriptResource() : Resource() {}
            ScriptResource(std::string sourceID) : Resource(sourceID) {}
            
            bool IsCritical() override;
            
            void Run();
            
        protected:
            void _load() override;
            void _unload() override;
        };
        
        class ImageResource : public Resource {
        public:
            ImageResource() : Resource() {}
            ImageResource(std::string sourceID);
            ImageResource(Texture* textureID);
            
            bool IsCritical() override;
            
            std::string GetName() override;
            
            Texture* GetTexture();
            
        protected:
            void _load() override;
            
        private:
            std::string _name;
            
            bool _manualTexture;
            
            Texture* _texture;
        };
        
        class FontResource : public Resource {
        public:
            FontResource() : Resource() {}
            FontResource(std::string sourceID) : Resource(sourceID) {}
            
            bool IsCritical() override;
            
            GLFT_Font* GetFont(unsigned int fontSize);
            
        protected:
            void _load() override;
            void _unload() override;
            
        private:
            std::map<unsigned int, GLFT_Font*> _fonts;
        };
        
        void Load(std::string path);
        void Load(std::string resourceID, Source* src);
        void UnloadAll();
        bool HasSource(std::string sourceID);
    }
}