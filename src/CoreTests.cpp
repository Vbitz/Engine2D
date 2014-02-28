#include "TestSuiteAPI.hpp"

#include "Events.hpp"
#include "Logger.hpp"

#include "Platform.hpp"

namespace Engine {
    
    static int adder = 0;
    
    void AdderTest1(Json::Value val) {
        if ((adder % 50000) == 0) {
			Logger::begin("Script", Logger::LogLevel_Log) << "[1] adder = " << adder << Logger::end();
		}
        adder++;
    }
    
    void AdderTest2(Json::Value val) {
        if ((adder % 50000) == 0) {
			Logger::begin("Script", Logger::LogLevel_Log) << "[2] adder = " << adder << Logger::end();
		}
        adder++;
    }
    
    class CoreEventTest : public Test {
    public:
        std::string GetName() override { return "CoreEventTest"; }
        
        void Setup() {
            adder = 0;
        }
        
        void Run() {
            double startTime = Platform::GetTime();
            
            Events::On("testingEvent1", "TestEvent1", AdderTest1);
            Events::On("testingEvent2", "TestEvent2", AdderTest2);
            
            double endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::On Performance Test x 2: " << (endTime - startTime) << "s" << Logger::end();
            if (endTime - startTime > 5.0e-05) {
                this->FailTest();
            }
            
            Events::Emit("testingEvent1");
            Events::Emit("testingEvent2");
            
            startTime = Platform::GetTime();
            
            Events::Emit("testingEvent1");
            Events::Emit("testingEvent2");
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::Emit Performance Test x 2: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 1.5e-05) {
            //    this->FailTest();
            //}
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 100000; i++) {
                Events::Emit("testingEvent1");
                Events::Emit("testingEvent2");
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::Emit Performance Test x 200000: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 0.5) {
            //    this->FailTest();
            //}
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 100000; i++) {
                Events::Emit("testingEvent1Fake");
                Events::Emit("testingEvent2Fake");
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::Emit No Target Performance Test x 200000: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 1.0e-1) {
            //    this->FailTest();
            //}
            
            // Make sure JS has time to laxy compile the methods
            Events::Emit("testingEventJS1");
            Events::Emit("testingEventJS2");
            
            startTime = Platform::GetTime();
            
            Events::Emit("testingEventJS1");
            Events::Emit("testingEventJS2");
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ -> JavaScript Event::Emit Performance Test x 2: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 1.0e-03) {
            //    this->FailTest();
            //}
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 100000; i++) {
                Events::Emit("testingEventJS1");
                Events::Emit("testingEventJS2");
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ -> JavaScript Event::Emit Performance Test x 100000: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 0.5) {
            //    this->FailTest();
            //}
        }
    };
    
    class CoreLoggerTest : public Test {
    public:
        
        std::string GetName() { return "CoreLoggerTest"; }
        
        void Run() {
            double startTime, endTime;
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 10; i++) {
                Logger::begin("Hello", Logger::LogLevel_Log) << "World" << Logger::end();
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreLoggerTest", Logger::LogLevel_Log) << "Logger::begin Performance Test x 10: " << (endTime - startTime) << "s" << Logger::end();
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 10; i++) {
                Logger::LogText("Hello", Logger::LogLevel_Log, "World");
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreLoggerTest", Logger::LogLevel_Log) << "Logger::LogText Performance Test x 10: " << (endTime - startTime) << "s" << Logger::end();
        }
    };
    
    void LoadCoreTests() {
        TestSuite::RegisterTest(new CoreEventTest());
        TestSuite::RegisterTest(new CoreLoggerTest());
    }
}