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