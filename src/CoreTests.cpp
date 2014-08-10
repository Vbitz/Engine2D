/*
   Filename: CoreTests.cpp
   Purpose:  Tests for the core module

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

#include "TestSuiteAPI.hpp"

#include "Events.hpp"
#include "Logger.hpp"

#include "Platform.hpp"

namespace Engine {
    
    static int adder = 0;
    
    EventMagic AdderTest1(Json::Value val, void* userPointer) {
        if ((adder % 50000) == 0) {
			Logger::begin("Script", Logger::LogLevel_Log) << "[1] adder = " << adder << Logger::end();
		}
        adder++;
        return EM_OK;
    }
    
    EventMagic AdderTest2(Json::Value val, void* userPointer) {
        if ((adder % 50000) == 0) {
			Logger::begin("Script", Logger::LogLevel_Log) << "[2] adder = " << adder << Logger::end();
		}
        adder++;
        return EM_OK;
    }
    
    class CoreEventTest : public Test {
    public:
        std::string GetName() override { return "CoreEventTest"; }
        
        void Setup() {
            adder = 0;
        }
        
        void Run() {
            double startTime = Platform::GetTime();
            
            EventClassPtrRef testingEvent1 =
                GetEventsSingilton()->GetEvent("testingEvent1");
            EventClassPtrRef testingEvent2 =
                GetEventsSingilton()->GetEvent("testingEvent1");
            EventClassPtrRef testingEvent1Fake =
                GetEventsSingilton()->GetEvent("testingEvent1Fake");
            EventClassPtrRef testingEvent2Fake =
                GetEventsSingilton()->GetEvent("testingEvent1Fake");
            EventClassPtrRef testingEventJS1 =
                GetEventsSingilton()->GetEvent("testingEventJS1");
            EventClassPtrRef testingEventJS2 =
                GetEventsSingilton()->GetEvent("testingEventJS1");
            
            testingEvent1->AddListener("TestEvent1", GetEventsSingilton()->MakeTarget(AdderTest1));
            testingEvent1->AddListener("TestEvent2", GetEventsSingilton()->MakeTarget(AdderTest2));
            
            double endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::On Performance Test x 2: " << (endTime - startTime) << "s" << Logger::end();
            if (endTime - startTime > 5.0e-05) {
                this->FailTest();
            }
            
            testingEvent1->Emit();
            testingEvent2->Emit();
            
            startTime = Platform::GetTime();
            
            testingEvent1->Emit();
            testingEvent2->Emit();
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::Emit Performance Test x 2: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 1.5e-05) {
            //    this->FailTest();
            //}
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 100000; i++) {
                testingEvent1->Emit();
                testingEvent2->Emit();
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::Emit Performance Test x 200000: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 0.5) {
            //    this->FailTest();
            //}
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 100000; i++) {
                testingEvent1Fake->Emit();
                testingEvent2Fake->Emit();
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ Event::Emit No Target Performance Test x 200000: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 1.0e-1) {
            //    this->FailTest();
            //}
            
            // Make sure JS has time to laxy compile the methods
            testingEventJS1->Emit();
            testingEventJS2->Emit();
            
            startTime = Platform::GetTime();
            
            testingEventJS1->Emit();
            testingEventJS2->Emit();
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ -> JavaScript Event::Emit Performance Test x 2: " << (endTime - startTime) << "s" << Logger::end();
            //if (endTime - startTime > 1.0e-03) {
            //    this->FailTest();
            //}
            
            startTime = Platform::GetTime();
            
            for (int i = 0; i < 100000; i++) {
                testingEventJS1->Emit();
                testingEventJS2->Emit();
            }
            
            endTime = Platform::GetTime();
            Logger::begin("CoreEventTest", Logger::LogLevel_Log) << "C++ -> JavaScript Event::Emit Performance Test x 200000: " << (endTime - startTime) << "s" << Logger::end();
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