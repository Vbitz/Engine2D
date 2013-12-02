#pragma once

#include <vector>
#include <string>

#include "Platform.hpp"

#include "TestSuiteAPI.hpp"

namespace Engine {
    namespace TestSuite {
        void LoadTestSuiteTests();
        
        void Run();
    }
}