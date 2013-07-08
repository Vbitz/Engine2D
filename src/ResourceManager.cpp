#include "ResourceManager.hpp"

namespace Engine {
    namespace ResourceManager {
        
        Source* _getSource(std::string sourceID);
        void _register(Resource* res);
        
        // Source
        unsigned char* Source::GetData(long &len) {
            if (this->_savedData == NULL || this->NeedsUpdate()) {
                this->_savedData = this->_getData(this->_savedDataLength);
            }
            len = this->_savedDataLength;
            return this->_savedData;
        }
        
        bool Source::NeedsUpdate() { return false; }
        
        std::string Source::GetName() {
            return "res://invalidSource";
        }
        
        unsigned char* Source::_getData(long& len) {
            len = 0;
            return new unsigned char[0];
        }
        
        // FileSource
        FileSource::FileSource(std::string path) {
            this->_path = path;
            this->_lastModify = -1;
        }
            
        unsigned char* FileSource::_getData(long& len) {
            this->_lastModify = Filesystem::GetFileModifyTime(this->_path);
            return (unsigned char*) Filesystem::GetFileContent(this->_path, len);
        }
        
        bool FileSource::NeedsUpdate() {
            return Filesystem::GetFileModifyTime(this->_path) > this->_lastModify;
        }
        
        std::string FileSource::GetName() {
            return "file://res/" + this->_path;
        }
        
        
        // WebSource
        WebSource::WebSource(std::string webRoot, std::string path) : _webRoot(webRoot), _path(path) {
            
        }
            
        unsigned char* WebSource::_getData(long& len) {
            return NULL;
        }
        
        bool WebSource::NeedsUpdate() {
            return false;
        }
        
        std::string WebSource::GetName() {
            return this->_webRoot + "/" + this->_path;
        }
        
        
        // Resource
        Resource::Resource() {
            Logger::LogText("ResourceManager", Logger::LogLevel_Error, "Invalid Resource Created");
            this->_source = NULL;
        }
        
        Resource::Resource(std::string sourceID) {
            if (sourceID == "") {
                this->_source = new DummySource();
            } else {
                this->_source = _getSource(sourceID);
            }
            
            Logger::begin("ResourceManager", Logger::LogLevel_Log)
                << "Loading Resource with sourceID: " << sourceID << " and name " << this->_source->GetName()
                << Logger::end();
            this->_loaded = false;
            
            if (this->IsCritical()) {
                this->Load();
            }
            
            _register(this);
        }
            
        bool Resource::IsCritical() {
            return true;
        }
        
        
        std::string Resource::GetName() {
            if (this->_source == NULL) {
                return "res://invalidResource";
            }
            return this->_source->GetName();
        }
        
        void Resource::Load() {
            if (this->_source == NULL) {
                return;
            }
            
            if (this->_loaded) {
                return;
            }
        
            Logger::begin("ResourceManager", Logger::LogLevel_Verbose) << "Loading Resource: " << this->GetName() << Logger::end();
            
            this->_load();
            
            this->_loaded = true;
        }
        
        void Resource::Unload() {
            if (this->_source == NULL) {
                return;
            }
            
            if (!this->_loaded) {
                return;
            }
            
            Logger::begin("ResourceManager", Logger::LogLevel_Verbose) << "Unloading Resource: " << this->GetName() << Logger::end();
            
            this->_unload();
            this->_loaded = false;
        }
        
        void Resource::CheckForUpdates(bool force) {
            if (this->_source == NULL) {
                return;
            }
            
            if (force || this->_source->NeedsUpdate()) {
                Logger::begin("ResourceManager", Logger::LogLevel_Log) << "Reloading Resource: " << this->GetName() << Logger::end();
                this->Unload();
                this->Load();
            }
        }
        
        bool Resource::IsLoaded() {
            if (this->_source == NULL) {
                return false;
            }
            return this->_loaded;
        }
        
        void Resource::_forceLoad() {
            if (this->_source == NULL) {
                return;
            }
            
            Logger::begin("ResourceManager", Logger::LogLevel_Warning) <<
                "Force Loading Resource: " << this->GetName() << " on First Use, this will incur a preformance penatily" <<
                Logger::end();
            
            std::string profilerZoneName = "ResourceManager_Force_" + this->GetName();
            Profiler::Begin(profilerZoneName); // tell the scripter/coder how long this loading really took
            this->_load();
            this->_loaded = true;
            Profiler::End(profilerZoneName);
        }
        
        void Resource::_load() { }
        
        void Resource::_unload() { }
        
        
        // ScriptResource
        bool ScriptResource::IsCritical() {
            return true;
        }
            
        void ScriptResource::Run() {
            if (!this->IsLoaded()) {
                this->_forceLoad();
            }
            
        }
        
        void ScriptResource::_load() {
            
        }
        
        void ScriptResource::_unload() {
            
        }
        
        ImageResource::ImageResource(std::string sourceName) : Resource(sourceName), _textureID(-1), _manualTexture(false) {
            this->_loaded = false;
            this->_name = this->_source->GetName();
        }
        
        // ImageResource
        ImageResource::ImageResource(GLuint textureID) : Resource(""), _textureID(textureID), _manualTexture(true) {
            this->_loaded = true;
            this->_name = "ManualImage_" + std::to_string(textureID);
        }
        
        bool ImageResource::IsCritical() {
            return false; // A missing texture is displayed until it's loaded
        }
        
        std::string ImageResource::GetName() {
            return this->_name;
        }
        
        GLuint ImageResource::GetTextureID() {
            if (!this->IsLoaded()) {
                this->_forceLoad();
            }
            return this->_textureID;
        }

        void ImageResource::_load() {
            // this method needs to get to the point where GetTextureID returns a valid texture;
            if (!this->_manualTexture) {
                long fileSize = 0; // Move this to Draw2D or some kind of TextureManager
                
                unsigned char* file = this->_source->GetData(fileSize);
                
                GLuint text;
                
                FIMEMORY* mem = FreeImage_OpenMemory((BYTE*) file, (unsigned int)fileSize);
                
                FIBITMAP *lImg = FreeImage_LoadFromMemory(FreeImage_GetFileTypeFromMemory(mem), mem);
                
                FIBITMAP *img = 0;
                
                if (FreeImage_GetBPP(lImg) == 24) {
                    Logger::begin("JSDraw", Logger::LogLevel_Warning) << "Converting image to 32bit" << Logger::end();
                    img = FreeImage_ConvertTo32Bits(lImg);
                } else {
                    img = lImg;
                }
                
                int width = FreeImage_GetWidth(img);
                int height = FreeImage_GetHeight(img);
                
                unsigned char* pixel = (unsigned char*)FreeImage_GetBits(img);
                unsigned char* texture = new unsigned char[4 * width * height];
                
                int pos = 0;
                
                for (int x = 0; x < width; x++) {
                    for (int y = 0; y < height; y++) {
                        texture[pos + 0] = pixel[pos + 2];
                        texture[pos + 1] = pixel[pos + 1];
                        texture[pos + 2] = pixel[pos + 0];
                        texture[pos + 3] = pixel[pos + 3];
                        pos += 4;
                    }
                }
                
                Draw2D::CheckGLError("Pre Image Load");
                
                glGenTextures(1, &text);
                
                glBindTexture(GL_TEXTURE_2D, text);
                
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST_MIPMAP_NEAREST );
                
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                GL_NEAREST );
                
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
                
                glGenerateMipmap(GL_TEXTURE_2D);
                
                glBindTexture(GL_TEXTURE_2D, 0);
                
                Draw2D::CheckGLError("Post Image Load");
                
                Logger::begin("JSDraw", Logger::LogLevel_Log) << "Created Image from file: " << text << Logger::end();
                
                this->_textureID = text;
                
                FreeImage_CloseMemory(mem);
                
                std::free(texture); // that should fix some anoying memory leaks
            } else {
                Logger::begin("ResourceManager", Logger::LogLevel_Warning) << "Manual Texture's can't be loaded" << Logger::end();
            }
        }
        
        void ImageResource::_unload() {
            glDeleteTextures(1, &this->_textureID);
        }
        
        
        // FontResource
        bool FontResource::IsCritical() {
            return false;
        }
            
        GLFT_Font* FontResource::GetFont(unsigned int fontSize) {
            if (!this->IsLoaded()) {
                this->_forceLoad();
            }
            if (this->_fonts.count(fontSize) == 0) {
                GLFT_Font* newFont = new GLFT_Font();
                long fileLen = 0;
                unsigned char* fileData = this->_source->GetData(fileLen);
                newFont->open((char*) fileData, fileLen, fontSize);
                this->_fonts[fontSize] = newFont;
                Logger::begin("ResourceManager", Logger::LogLevel_Verbose) << "Loaded Font: " << this->_source->GetName() << " with size " << fontSize << Logger::end();
            }
            return this->_fonts[fontSize];
        }
        
        void FontResource::_load() { } // fonts are loaded on demand
        
        void FontResource::_unload() {
            for (auto iter = this->_fonts.begin(); iter != this->_fonts.end(); iter++) {
                iter->second->release();
            }
            this->_fonts.clear();
        }
        
        std::vector<Resource*> _resources;
        std::map<std::string, Source*> _sources;
        
        // ResourceManager
        Source* _getSource(std::string sourceID) {
            if (_sources.count(sourceID) == 0) {
                Logger::begin("ResourceManager", Logger::LogLevel_Error) << "Source: " << sourceID << " does not exist" << Logger::end();
                return NULL;
            }
            return _sources[sourceID];
        }
        
        void _register(Resource* res) {
            _resources.push_back(res);
        }
        
        void Load(std::string path) {
            Logger::begin("ResourceMamager", Logger::LogLevel_Verbose) << "Loading Source: " << path << Logger::end();
            _sources[path] = new FileSource(path);
        }
        
        void Load(std::string resourceID, Source src) {
            Logger::begin("ResourceMamager", Logger::LogLevel_Verbose) << "Loading Source: " << resourceID << " with name " << src.GetName() << Logger::end();
            _sources[resourceID] = &src;
        }
        
        void UnloadAll() {
            for (auto iter = _resources.begin(); iter != _resources.end(); iter++) {
                Resource* res = *iter;
                res->Unload();
            }
            _resources.clear();
            for (auto iter2 = _sources.begin(); iter2 != _sources.end(); iter2++) {
                iter2->second->Unload();
            }
            _sources.clear();
        }
        
        bool HasSource(std::string sourceID) {
            return _sources.count(sourceID) != 0;
        }
    }
}