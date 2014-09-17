/*
   Filename: Profiler.cpp
   Purpose:  Code performance profiler

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

#include "Profiler.hpp"

#include <functional>
#include <unordered_map>
#include <sstream>
#include <map>
#include <stack>

#include "vendor/json/json.h"

#include "Logger.hpp"
#include "Config.hpp"
#include "Events.hpp"

#include "Platform.hpp"

namespace Engine {
    namespace Profiler {
        
        ProfileZoneMetadata *lastRootZone, *rootZone, *currentZone;
        
        void BeginProfile(ScopePtr scope) {
            if (currentZone == NULL) return;
            if (!Platform::IsMainThread()) return;
            std::string scopeName = scope->GetName();
            if (currentZone->children.count(scopeName) == 0) {
                ProfileZoneMetadata* newZone = new ProfileZoneMetadata();
                newZone->name = scopeName;
                newZone->parent = currentZone;
                currentZone->children[scopeName] = newZone;
                currentZone = newZone;
            } else {
                currentZone = currentZone->children[scopeName];
            }
        }
        
        void SubmitProfile(ScopePtr scope) {
            if (currentZone == NULL) return;
            if (!Platform::IsMainThread()) return;
            assert(currentZone->parent != NULL);
            double time = scope->GetElapsedTime();
            currentZone->callCount++;
            currentZone->totalTime += time;
            currentZone->totalSelfTime += time;
            if (currentZone->avgTime == -1) {
                currentZone->avgTime = time;
            } else {
                currentZone->avgTime += (time - currentZone->avgTime) / currentZone->callCount;
            }
            if (time > currentZone->maxTime) {
                currentZone->maxTime = time;
            }
            if (time < currentZone->minTime) {
                currentZone->minTime = time;
            }
            currentZone->parent->totalSelfTime -= time;
            currentZone = currentZone->parent;
        }
        
        void BeginProfileFrame() {
            if (!Platform::IsMainThread()) return;
            rootZone = currentZone = new ProfileZoneMetadata();
            currentZone->name = "Root";
        }
        
        void _freeZone(ProfileZoneMetadata* zone) {
            if (!Platform::IsMainThread()) return;
            for (auto iter = zone->children.begin(); iter != zone->children.end(); iter++) {
                _freeZone(iter->second);
            }
            delete zone;
        }
        
        void _buildJSONFromZone(ProfileZoneMetadata* zone, Json::Value& ret) {
            ret["name"] = zone->name;
            ret["count"] = zone->callCount;
            ret["total"] = zone->totalTime;
            ret["self"] = zone->totalSelfTime;
            ret["avg"] = zone->avgTime;
            ret["min"] = zone->minTime;
            ret["max"] = zone->maxTime;
            Json::Value& children = ret["children"] = Json::objectValue;
            for (auto iter = zone->children.begin(); iter != zone->children.end(); iter++) {
                _buildJSONFromZone(iter->second, children[iter->first]);
            }
        }
        
        void EndProfileFrame() {
            if (!Platform::IsMainThread()) return;
            assert(currentZone == rootZone);            
            if (lastRootZone != NULL) {
                _freeZone(lastRootZone);
            }
            lastRootZone = currentZone;
            if (GetEventsSingilton()->GetEvent("onProfileEnd")->ListenerCount() > 0) {
                GetEventsSingilton()->GetEvent("onProfileEnd")->Emit(GetLastFrame());
            }
            currentZone = rootZone = NULL;
        }
        
        Json::Value GetLastFrame() {
            Json::Value args(Json::objectValue);
            _buildJSONFromZone(lastRootZone, args);
            return args;
        }
    }
}