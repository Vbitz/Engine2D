/*
   Filename: Addon.hpp
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

#pragma once

#include <unordered_map>
#include <string>

#include "Engine2D.hpp" // Public Header file

#include "vendor/json/json.h"
#include "stdlib.hpp"

#include "Platform.hpp"

namespace Engine {
    ENGINE_CLASS(Addon);
    
    class Addon {
    public:
        Addon(AddonSpec spec) : _spec(spec) { }
        
        void GetName() {
            return this->_spec.name;
        }
        
        void GetAuthor() {
            return this->_spec.author;
        }
        
        void DoStartup();
        void DoShutdown();
        
        static void Load(std::string filename);
        
        static void LoadAll(LoadOrder load);
        
        static void Register(AddonSpec spec);
        static void Shutdown();
    private:
        AddonSpec _spec;
        bool _loaded = false;
    };
}