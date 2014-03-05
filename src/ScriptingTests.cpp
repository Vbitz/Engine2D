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
        TestSuite::RegisterTest(new ScriptingLoaderTest());
    }
}