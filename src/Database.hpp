/*
   Filename: Database.hpp
   Purpose:  Basic C++ interface for querying sqlite3

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

#pragma once

#include <vector>
#include <map>

#include "Logger.hpp"
#include "stdlib.hpp"
    
namespace Engine {
    ENGINE_CLASS(Database);
    
    class Database {
    public:
        virtual ~Database() {}
        
        virtual bool Execute(std::string statement) = 0;
        virtual std::vector< std::map<std::string, std::string> > ExecutePrepare(std::string statement) = 0;
        
        static DatabasePtr CreateDatabase(std::string filename);
    };
}