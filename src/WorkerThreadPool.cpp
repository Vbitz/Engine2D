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
#include "Events.hpp"

#include "JSSys.hpp"

namespace Engine {
    namespace WorkerThreadPool {
        
        ENGINE_JS_METHOD(ThreadLog) {
			ENGINE_JS_SCOPE_OPEN;
            
            std::stringstream logStr;
            
			bool first = true;
			for (int i = 0; i < args.Length(); i++) {
				if (first) {
					first = false;
				} else {
					logStr << " ";
				}
				v8::String::Utf8Value str(args[i]->ToString());
				const char* cstr = *str;
				logStr << cstr;
			}
        
            Logger::LogText(std::string(*v8::String::Utf8Value(args.Data()->ToString())), Logger::LogLevel_User, logStr.str());
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
		}
        
        ENGINE_JS_METHOD(ThreadEventEmit) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the event to target");
            ENGINE_CHECK_ARG_OBJECT(1, "Arg1 is the object to submit to the event handler");
            
            Events::EmitThread(
                    std::string(*v8::String::Utf8Value(args.Data()->ToString())),
                    ENGINE_GET_ARG_CPPSTRING_VALUE(0),
                    ScriptingManager::ObjectToJson(args[1].As<v8::Object>()));
            
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
            
#define addItem(table, js_name, funct) table->Set(this->_isolate, js_name, v8::FunctionTemplate::New(this->_isolate, funct))
            
            void CreateScriptContext(std::string threadID) {
                // ONLY called from Worker thread, this will kill scripting if called from the main thread
                this->_isolate = v8::Isolate::New();
                this->_isolate->Enter();
                
                v8::HandleScope scp(this->_isolate);
                
                v8::Handle<v8::ObjectTemplate> globals = v8::ObjectTemplate::New();
                
                v8::Handle<v8::FunctionTemplate> threadLog = v8::FunctionTemplate::New(this->_isolate);
                
                std::stringstream threadIDSS;
                
                threadIDSS << "WorkerThread {" << threadID.substr(0, 13) << "}";
                
                threadLog->SetCallHandler(ThreadLog, v8::String::NewFromUtf8(this->_isolate, threadIDSS.str().c_str()));
                
                globals->Set(this->_isolate, "log", threadLog);
                
                addItem(globals, "sleep", ThreadSleep);
                
                v8::Handle<v8::FunctionTemplate> threadEventEmit = v8::FunctionTemplate::New(this->_isolate);
                
                threadEventEmit->SetCallHandler(ThreadEventEmit, v8::String::NewFromUtf8(this->_isolate, threadIDSS.str().c_str()));
                
                globals->Set(this->_isolate, "emit", threadEventEmit);
                
                this->_globalTemplate.Reset(this->_isolate, globals);
                
                v8::Handle<v8::Context> context = v8::Context::New(this->_isolate);
                
                context->Enter();
                
                this->_context.Reset(this->_isolate, context);
            }
            
#undef addItem
            
            bool RunScript(std::string src, std::string fileName) {
                std::stringstream realSrc;
                
                realSrc << "(" << src << ")"; // needed to return the variable
                
                // Only called from Worker thread
                v8::HandleScope scp(this->_isolate);
                v8::Handle<v8::Context> ctx = v8::Local<v8::Context>::New(this->_isolate, _context);
                v8::Context::Scope ctxScope(ctx);
                
                v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(this->_isolate, realSrc.str().c_str()), v8::String::NewFromUtf8(this->_isolate, fileName.c_str()));
                
                if (script.IsEmpty()) {
                    return false;
                } else {
                    v8::Handle<v8::Value> rawFunc = script->Run();
                    v8::Handle<v8::Function> func = rawFunc.As<v8::Function>();
                    
                    v8::Handle<v8::Value> args[1];
                    
                    v8::Handle<v8::ObjectTemplate> globalObject = v8::Local<v8::ObjectTemplate>::New(this->_isolate, this->_globalTemplate);
                    
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
        
        std::vector<ScriptWorker*> _workers;
        
        void* ScriptWorkerFunc(void* scriptWorkerArgs) {
            ScriptWorkerArgs* args = (ScriptWorkerArgs*) scriptWorkerArgs;
            
            args->threadIDMutex->Enter();
            
            unsigned char* threadID = args->threadID;
            
            args->threadIDMutex->Exit();
            
            std::string strThreadID = Platform::StringifyUUID(args->threadID);
            
            args->worker->CreateScriptContext(strThreadID);
            args->worker->RunScript(args->scriptSource, strThreadID);
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
            
            _workers.push_back(args->worker);
            
            Logger::begin("WorkerThreadPool", Logger::LogLevel_Log) << "Created ScriptWorkerThread {" << Platform::StringifyUUID(args->threadID) << "}" << Logger::end();
        }
    }
}