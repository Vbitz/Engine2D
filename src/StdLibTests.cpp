#include "StdLibTests.hpp"
#include "TestSuiteAPI.hpp"

#include "stdlib.hpp"
#include "Logger.hpp"

namespace Engine {
    
    class StringTest : public Test {
        void Run() override {
            Engine::String s = "Hello World";
            Logger::begin("StringTest", Logger::LogLevel_Log) << "String = '" << s << "' : String.Length = " << (int) s.length() << Logger::end();
        }
        
        std::string GetName() override { return "StringTest"; }
    };
    
    void LoadStdLibTests() {
        TestSuite::RegisterTest(new StringTest());
    }
}