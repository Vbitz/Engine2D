#include "TestSuite.hpp"

#include "Logger.hpp"

namespace Engine {
    
    void Test::Assert(std::string name, bool value) {
        if (!value) {
            this->FailTest();
            Logger::begin("TestSuite", Logger::LogLevel_TestError)
                << "FAILED in " + this->GetName() + " : " << name << Logger::end();
        }
    }
    
    void Test::FailTest() {
        this->_failed = true;
    }
    
    bool Test::GetFailed() {
        return this->_failed;
    }
    
    class PassTestTest : public Test {
    public:
        void Run() override { }
        
        std::string GetName() override {
            return "PassTest";
        }
    };
    
    class FailTestTest : public Test {
    public:
        void Run() override {
            this->Assert("Fail", false);
        }
        
        std::string GetName() override {
            return "FailTest";
        }
    };
    
    namespace TestSuite {
        
        std::vector<Test*> _tests;
        
        void RegisterTest(Test* t) {
            _tests.push_back(t);
        }
        
        void LoadTestSuiteTests() {
            _tests.push_back(new PassTestTest());
            _tests.push_back(new FailTestTest());
        }
        
        void Run() {
            int passedTests = 0,
                failedTests = 0;
            
            Logger::begin("TestSuite", Logger::LogLevel_TestLog)
                << "TestSuite Starting" << Logger::end();
            
            for (auto iter = _tests.begin(); iter != _tests.end(); iter++) {
                Test* t = *iter;
                
                t->Setup();
                
                Logger::begin("TestSuite", Logger::LogLevel_TestLog)
                    << "Running Test : " << t->GetName() << Logger::end();
                
                t->Run();
                
                if (t->GetFailed()) {
                    failedTests++;
                    Logger::begin("TestSuite", Logger::LogLevel_TestError)
                        << "FAILED TEST : " << t->GetName() << Logger::end();
                } else {
                    passedTests++;
                    Logger::begin("TestSuite", Logger::LogLevel_TestLog)
                        << "PASSED TEST : " << t->GetName() << Logger::end();
                }
                
                t->PullDown();
            }
            
            Logger::begin("TestSuite", Logger::LogLevel_TestLog) << "TestSuite Finished "
                << passedTests << " passed : " << failedTests << " failed" << Logger::end();
        }
        
    }
}