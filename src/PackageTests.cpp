/*
 Filename: PlatformTests.cpp
 Purpose:  Tests for the platform module
 
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

#include "Package.hpp"
#include "TestSuiteAPI.hpp"

#include <cstdlib>
#include <cstring>

namespace Engine {
    
    class BasicPackageTest : public Test {
    public:
        std::string GetName() override { return "BasicPackageTest"; }
        
        void Run() {
            double startTime = Platform::GetTime();
            if (Filesystem::FileExists("testing.epkg")) {
                Filesystem::DeleteFile("testing.epkg");
            }
            PackagePtr p = Package::FromFile("testing.epkg");
            
            std::string content = "Hello, World";
            
            unsigned char* fileData1 = (unsigned char*) content.c_str();
            uint32_t fileLength1 = strlen((char*) fileData1);
            
            p->WriteFile("testing.txt", fileData1, fileLength1, Package::DefaultFileFlags);
            
            uint8_t* randomBytes = new uint8_t[21341];
            std::memset(randomBytes, 0xFEEDFACE, 21341);
            
            for (int i = 0; i < 64; i++) {
                std::string filename = std::to_string(i) + ".test";
                p->WriteFile(filename, randomBytes, 21341, Package::CompressedFileFlags);
            }
            
            p->GetIndex()["hello"] = "World";
            
            p->Close();
            
            PackagePtr p2 = Package::FromFile("testing.epkg");
            
            uint32_t fileLength2 = 0;
            uint8_t* fileData2 = p2->ReadFile("testing.txt", fileLength2);
            this->Assert("Check File Length", fileLength2 == fileLength1);
            this->Assert("Check File Content", content == std::string((char*) fileData2, fileLength2));
            
            uint32_t fileLength3 = 0;
            uint8_t* fileData3 = p2->ReadFile("1.test", fileLength3);
            this->Assert("Check Compressed File Length", fileLength3 == 21341);
            this->Assert("Check Compressed File Content", fileData3[0] == randomBytes[0]);
            
            this->Assert("Check File Index Content", p2->GetIndex()["hello"].asString() == "World");
            
            double endTime = Platform::GetTime();
            
            std::cout << "time = " << (endTime - startTime) << "s" << std::endl;
        }
    };
    
    void LoadPackageTests() {
        TestSuite::RegisterTest(new BasicPackageTest());
    }
}
