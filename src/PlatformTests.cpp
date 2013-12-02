#include "Platform.hpp"
#include "TestSuiteAPI.hpp"

namespace Engine {
    
    class PlatformStackTrace : public Test {
    public:
        std::string GetName() override { return "PlatformStackTrace"; }
        
        void Run() {
            Platform::DumpStackTrace();
        }
    };
    
    void LoadPlatformTests() {
        TestSuite::RegisterTest(new PlatformStackTrace());
    }
}