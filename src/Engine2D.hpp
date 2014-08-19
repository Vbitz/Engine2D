/*
 Filename: Engine2D.hpp
 Purpose:  Public header file
 
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

#define ENGINE_CONSTRUCTOR __attribute__((constructor))

#ifndef _BUILD_UUID
#define _BUILD_UUID "INTERNAL_ADDON"
#endif

namespace Engine {
    using AddonStartup = void (*) ();
    using AddonShutdown = void (*) ();

    enum class LoadOrder : short {
        Startup = 0,
        PreScript = 100,
        PreGraphics = 200,
        Loaded = 300,
        FirstFrame = 400
    };
    
    struct AddonSpec {
        const char* uuid;
        const char* name;
        const char* author;
        AddonStartup startup;
        AddonShutdown shutdown;
        LoadOrder load;
    };
    
    void RegisterAddon(AddonSpec spec);
}
    
extern "C" {
    __attribute__ ((visibility("default"))) void RegisterCAddon(Engine::AddonSpec spec);
}

#define ENGINE_ADDON(Name, Author, Startup, Shutdown, LoadOrder) \
namespace { \
    Engine::AddonSpec Addon = { \
        _BUILD_UUID, \
        Name, \
        Author, \
        Startup, \
        Shutdown, \
        LoadOrder \
    }; \
\
    ENGINE_CONSTRUCTOR void Addon_Init() { \
        Engine::RegisterAddon(Addon); \
    } \
}