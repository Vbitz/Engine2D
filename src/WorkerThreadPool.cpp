/*
 Filename: WorkerThreadPool.cpp
 Purpose:  Coordicate threading of workers based around the event subsystem
 
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

#include "WorkerThreadPool.hpp"

#include <vector>

#include "Logger.hpp"
#include "Util.hpp"

#include "JSSys.hpp"

namespace Engine {
    namespace WorkerThreadPool {
        
        ENGINE_JS_METHOD(RawLog) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            std::cout << ENGINE_GET_ARG_CPPSTRING_VALUE(0) << std::endl;
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        ENGINE_JS_METHOD(ThreadSleep) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_NUMBER(0, "Arg0 is how long to sleep the thread for");
            
            Platform::NanoSleep((int) (ENGINE_GET_ARG_NUMBER_VALUE(0) * 1000000));
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
        class ScriptWorker {
        public:
            void Start() { // Can be called anywhere
                this->_running = true;
            }
            
            void Stop() { // Can only be called from main thread
                this->_running = false;
            }
            
            bool IsRunning() {
                // Can be called anywhere, _running get is a atomic op
                return this->_running;
            }
            
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
            
            void CreateScriptContext() {
                // ONLY called from Worker thread, this will kill scripting if called from the main thread
                this->_isolate = v8::Isolate::New();
                this->_isolate->Enter();
                
                v8::HandleScope scp(this->_isolate);
                
                v8::Handle<v8::ObjectTemplate> globals = v8::ObjectTemplate::New();
                
                addItem(globals, "log", RawLog);
                addItem(globals, "sleep", ThreadSleep);
                
                this->_globalTemplate.Reset(this->_isolate, globals);
                
                v8::Handle<v8::Context> context = v8::Context::New(this->_isolate);
                
                context->Enter();
                
                this->_context.Reset(this->_isolate, context);
            }
            
#undef addItem
            
            bool RunScript(std::string src, std::string fileName) {
                std::stringstream realSrc;
                
                realSrc << "(" << src << ")";
                
                // Only called from Worker thread
                v8::HandleScope scp(this->_isolate);
                v8::Handle<v8::Context> ctx = v8::Handle<v8::Context>::New(this->_isolate, _context);
                v8::Context::Scope ctxScope(ctx);
                
                v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(realSrc.str().c_str()), v8::String::New(fileName.c_str()));
                
                if (script.IsEmpty()) {
                    return false;
                } else {
                    v8::Handle<v8::Value> rawFunc = script->Run();
                    v8::Handle<v8::Function> func = rawFunc.As<v8::Function>();
                    
                    v8::Handle<v8::Value> args[1];
                    
                    v8::Handle<v8::ObjectTemplate> globalObject = v8::Handle<v8::ObjectTemplate>::New(this->_isolate, this->_globalTemplate);
                    
                    args[0] = globalObject->NewInstance();
                    
                    func->Call(ctx->Global(), 1, args);
                    
                    return true;
                }
            }
            
        private:
            bool _running = false;
            v8::Isolate* _isolate;
            v8::Persistent<v8::ObjectTemplate> _globalTemplate;
            v8::Persistent<v8::Context> _context;
        };
        
        struct ScriptWorkerArgs {
            std::string scriptSource;
            unsigned char* threadID;
            
            ScriptWorker* worker = NULL;
            
            Platform::Mutex* threadIDMutex = NULL;
        };
        
        std::vector<ScriptWorker> _workers;
        
        void* ScriptWorkerFunc(void* scriptWorkerArgs) {
            ScriptWorkerArgs* args = (ScriptWorkerArgs*) scriptWorkerArgs;
            
            args->threadIDMutex->Enter();
            
            unsigned char* threadID = args->threadID;
            
            args->threadIDMutex->Exit();
            
            args->worker->CreateScriptContext();
            args->worker->RunScript(args->scriptSource, Platform::StringifyUUID(args->threadID));
            args->worker->Start();
            
            while (args->worker->IsRunning()) {
                // TODO: Recieve events using Events::PollThreadWorker(threadID)
                // TODO: Dispatch events
                Platform::Sleep(0);
            }
        }
        
        void CreateScriptWorker(std::string scriptSource) {
            ScriptWorkerArgs* args = new ScriptWorkerArgs;
            args->scriptSource = scriptSource;
            args->worker = new ScriptWorker();
            args->threadIDMutex = Platform::CreateMutex();
            args->threadIDMutex->Enter();
            Platform::Thread* thread =
                Platform::CreateThread(ScriptWorkerFunc, args);
            args->threadID = thread->GetThreadID();
            args->threadIDMutex->Exit();
            
            Logger::begin("WorkerThreadPool", Logger::LogLevel_Log) << "Created ScriptWorkerThread {" << Platform::StringifyUUID(args->threadID) << "}" << Logger::end();
        }
    }
}