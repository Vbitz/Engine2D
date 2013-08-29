#pragma once

#include "common.hpp"
#include "Draw2D.hpp"
#include "Filesystem.hpp"

#include "extern/GLFT_Font.hpp"

#include <FreeImage.h>

/*
 Think of Resources like this, If you could put it on the filesystem then it's a Source otherwise it's a Resource
 */
 
namespace Engine {
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
            ImageResource(GLuint textureID);
            
            bool IsCritical() override;
            
            std::string GetName() override;
            
            GLuint GetTextureID();
            
        protected:
            void _load() override;
            void _unload() override;
            
        private:
            std::string _name;
            
            bool _manualTexture;
            
            GLuint _textureID;
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