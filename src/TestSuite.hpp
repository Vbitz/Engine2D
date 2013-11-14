#pragma once

#include <vector>
#include <string>

#include "Platform.hpp"

#include "TestSuiteAPI.hpp"

namespace Engine {
    namespace TestSuite {
        void RegisterTest(Test* t);
        
        void Run();
    }
}