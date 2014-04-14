/*
   Filename: ScriptingTests.cpp
   Purpose:  Tests for the scripting module

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

#include "ScriptingTests.hpp"

#include "TestSuiteAPI.hpp"

#include "Logger.hpp"
#include "ScriptingManager.hpp"

namespace Engine {
    class ScriptingLoaderTest : public Test {
    public:
        std::string GetName() override {
            return "ScriptingLoaderTest";
        }
        
        static void SimpleLog(ScriptingManager::FunctionCallbackArgs& args) {
            Logger::begin("ScriptingLoaderTest", Logger::LogLevel_User) << args[0]->GetStringValue() << Logger::end();
        }
        
        void Run() override {
            ScriptingManager::ScriptingContext* context = ScriptingManager::CreateScriptingContext("v8");
            context->Create();
            ScriptingManager::ScriptingObject* console = context->CreateObject(ScriptingManager::ObjectType_Object);
            context->Set("console", console);
            console->Set("log", context->CreateFunction(SimpleLog));
            context->RunString("console.log(\"hello world\");", "testingScript");
        }
    };
    
    void LoadScriptingTests() {
        //TestSuite::RegisterTest(new ScriptingLoaderTest());
    }
}