/*
   Filename: ResourceManager.cpp
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

#include "ResourceManager.hpp"

#include "Profiler.hpp"

namespace Engine {
    namespace ResourceManager {
        
        SourcePtr _getSource(std::string sourceID);
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
        
        
        // RawSource
        RawSource::RawSource(const unsigned char* data, long fileLength) {
            this->data = (unsigned char*) data;
            this->fileLength = fileLength;
        }
        
        unsigned char* RawSource::_getData(long &len) {
            len = this->fileLength;
            return this->data;
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
            
            Logger::begin("ResourceManager", Logger::LogLevel_Verbose)
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
                Logger::begin("ResourceManager", Logger::LogLevel_Verbose) << "Reloading Resource: " << this->GetName() << Logger::end();
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
        
        ImageResource::ImageResource(std::string sourceName) : Resource(sourceName), _manualTexture(false) {
            this->_loaded = false;
            this->_name = this->_source->GetName();
        }
        
        // ImageResource
        ImageResource::ImageResource(TexturePtr t) : Resource(""), _manualTexture(true) {
            this->_loaded = true;
            this->_texture = t;
            this->_name = "ManualImage";
        }
        
        bool ImageResource::IsCritical() {
            return false; // A missing texture is displayed until it's loaded
        }
        
        std::string ImageResource::GetName() {
            return this->_name;
        }
        
        TexturePtr ImageResource::GetTexture() {
            if (!this->IsLoaded()) {
                this->_forceLoad();
            }
            return this->_texture;
        }

        void ImageResource::_load() {
            // this method needs to get to the point where GetTextureID returns a valid texture;
            if (!this->_manualTexture) {
                long fileSize = 0;
                
                unsigned char* file = this->_source->GetData(fileSize);
                
                this->_texture = ImageReader::TextureFromFileBuffer(file, fileSize);
            } else {
                Logger::begin("ResourceManager", Logger::LogLevel_Warning) << "Manual Texture's can't be loaded" << Logger::end();
            }
        }
        
        std::vector<ResourcePtr> _resources;
        std::map<std::string, SourcePtr> _sources;
        
        // ResourceManager
        SourcePtr _getSource(std::string sourceID) {
            if (_sources.count(sourceID) == 0) {
                Logger::begin("ResourceManager", Logger::LogLevel_Error) << "Source: " << sourceID << " does not exist" << Logger::end();
                return NULL;
            }
            return _sources[sourceID];
        }
        
        void _register(ResourcePtr res) {
            _resources.push_back(res);
        }
        
        void Load(std::string path) {
            Logger::begin("ResourceMamager", Logger::LogLevel_Verbose) << "Loading Source: " << path << Logger::end();
            _sources[path] = new FileSource(path);
        }
        
        void Load(std::string resourceID, Source* src) {
            Logger::begin("ResourceMamager", Logger::LogLevel_Verbose) << "Loading Source: " << resourceID << " with name " << src->GetName() << Logger::end();
            _sources[resourceID] = src;
        }
        
        void UnloadAll() {
            for (auto iter = _resources.begin(); iter != _resources.end(); iter++) {
                ResourcePtr res = *iter;
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