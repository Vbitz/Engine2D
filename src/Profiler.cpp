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
    namespace Profiler_New {
        
        ProfileZoneMetadata *rootZone, *currentZone;
        
        void BeginProfile(ScopePtr scope) {
            if (currentZone == NULL) return;
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
            assert(currentZone->parent != NULL);
            double time = scope->GetElapsedTime();
            currentZone->callCount++;
            currentZone->totalTime += time;
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
            currentZone = currentZone->parent;
        }
        
        void BeginProfileFrame() {
            rootZone = currentZone = new ProfileZoneMetadata();
            currentZone->name = "Root";
        }
        
        void _freeZone(ProfileZoneMetadata* zone) {
            for (auto iter = zone->children.begin(); iter != zone->children.end(); iter++) {
                _freeZone(iter->second);
            }
            delete zone;
        }
        
        Json::Value _buildJSONFromZone(ProfileZoneMetadata* zone) {
            Json::Value ret(Json::objectValue);
            ret["name"] = zone->name;
            ret["count"] = zone->callCount;
            ret["total"] = zone->totalTime;
            ret["avg"] = zone->avgTime;
            ret["min"] = zone->minTime;
            ret["max"] = zone->maxTime;
            Json::Value children(Json::objectValue);
            for (auto iter = zone->children.begin(); iter != zone->children.end(); iter++) {
                children[iter->first] = _buildJSONFromZone(iter->second);
            }
            ret["children"] = children;
            return ret;
        }
        
        void EndProfileFrame() {
            assert(currentZone == rootZone);
            if (GetEventsSingilton()->GetEvent("onProfileEnd")->ListenerCount() > 0) {
                Json::Value args(Json::objectValue);
                args["results"] = _buildJSONFromZone(rootZone);
                GetEventsSingilton()->GetEvent("onProfileEnd")->Emit(args);
            }
            _freeZone(rootZone);
        }
    }
}