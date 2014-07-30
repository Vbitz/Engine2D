/*
   Filename: Addon.cpp
   Purpose:  Public C++ addon support

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

#include "Addon.hpp"

#include <iostream>

#include "Application.hpp"

namespace Engine {
    std::unordered_map<std::string, AddonPtr>* _addons = NULL;
    std::unordered_map<std::string, Platform::LibaryPtr>* _loadedLibs = NULL;
    
    void Addon::DoStartup() {
        assert(!this->_loaded);
        Logger::begin("Addon", Logger::LogLevel_Verbose) << "Loading Addon \"" << this->_spec.name << "\" by \"" << this->_spec.author << "\"" << Logger::end();
        this->_spec.startup();
        this->_loaded = true;
    }
    
    void Addon::DoShutdown() {
        assert(this->_loaded);
        if (this->_spec.shutdown != NULL)
            this->_spec.shutdown();
    }
    
    void Addon::Load(std::string filename) {
        
    }
    
    void Addon::LoadAll(LoadOrder load) {
        if (_addons == NULL) {
            _addons = new std::unordered_map<std::string, AddonPtr>;
        }
        for (auto iter = _addons->begin();
             iter != _addons->end(); iter++) {
            if (!iter->second->_loaded && iter->second->_spec.load <= load) {
                iter->second->DoStartup();
            }
        }
    }
    
    void Addon::Register(AddonSpec spec) {
        if (_addons == NULL) {
            _addons = new std::unordered_map<std::string, AddonPtr>;
        }
        _addons->operator[](spec.uuid) = new Addon(spec);
        if (spec.load <= GetAppSingilton()->GetCurrentLoadState()) {
            _addons->operator[](spec.uuid)->DoStartup();
        }
    }
    
    void Addon::Shutdown() {
        if (_addons == NULL) {
            _addons = new std::unordered_map<std::string, AddonPtr>;
        }
        for (auto iter = _addons->begin();
             iter != _addons->end(); iter++) {
            if (iter->second->_loaded) {
                iter->second->DoShutdown();
            }
        }
    }
    
    void RegisterAddon(AddonSpec spec) {
        Addon::Register(spec);
    }
    
    void BasicAddon_Startup() {
        Logger::begin("BasicAddon", Logger::LogLevel_Log) << "BasicAddon Loaded" << Logger::end();
    }
    
    ENGINE_ADDON("BasicAddon", "Engine2D", BasicAddon_Startup, NULL, LoadOrder::PreScript);
}