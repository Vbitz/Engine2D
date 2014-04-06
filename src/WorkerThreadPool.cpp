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

namespace Engine {
    namespace WorkerThreadPool {
        
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
            
            void CreateScriptContext() {
                // Only called from Worker thread
                // TODO: Init V8 context with a new v8::Isolate
                // TODO: Build globals from JSWorker.cpp
            }
            
            void RunScript(std::string src) {
                // Only called from Worker thread
                // TODO: Run src as a new script in the thread v8 context
            }
            
        private:
            bool _running = false;
        };
        
        struct ScriptWorkerArgs {
            std::string scriptSource;
            int threadID;
            
            ScriptWorker* worker;
        };
        
        std::vector<ScriptWorker> _workers;
        
        void* ScriptWorkerFunc(void* scriptWorkerArgs) {
            ScriptWorkerArgs* args = (ScriptWorkerArgs*) scriptWorkerArgs;
            args->worker->CreateScriptContext();
            args->worker->RunScript(args->scriptSource);
            args->worker->Start();
            
            // Enter Event loop
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
            // TODO: Enter threadID mutex
            Platform::Thread* thread =
                Platform::CreateThread(ScriptWorkerFunc, args);
            // TODO: Add threadID to args->threadID
            // TODO: Exit threadID mutex
        }
    }
}