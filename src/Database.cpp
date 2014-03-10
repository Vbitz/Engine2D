/*
   Filename: Database.cpp
   Purpose:  Basic C++ interface to querying sqlite3

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

#include "Database.hpp"

#include "vendor/sqlite3.h"

namespace Engine {
    sqlite3* _database;
    
    bool hasInited = false;
    
    Database::Database(std::string database) {
        if (!hasInited) {
            Logger::begin("Sqlite", Logger::LogLevel_Verbose) << "Initalise SQLITE" << Logger::end();
            sqlite3_initialize();
            hasInited = true;
        }
        int result = sqlite3_open_v2(database.c_str(), &_database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if (result != SQLITE_OK) {
            Logger::begin("Sqlite", Logger::LogLevel_Error) << "Failed opening database: " << sqlite3_errmsg(_database) << Logger::end();
        }
    }
    
    Database::~Database() {
        sqlite3_close(_database);
    }
    
    bool Database::Execute(std::string statement) {
        char* errmsg = 0;
        int result = sqlite3_exec(_database, statement.c_str(), NULL, NULL, &errmsg);
        if (result != SQLITE_OK) {
            Logger::begin("Sqlite", Logger::LogLevel_Error) << "SQL Error : " << errmsg << Logger::end();
            return false;
        }
        return true;
    }
    
    std::vector< std::map<std::string, std::string> > Database::ExecutePrepare(std::string statement) {
        std::vector< std::map<std::string, std::string> > ret;
        sqlite3_stmt *sql;
        int result = sqlite3_prepare_v2(_database, statement.c_str(), -1, &sql, 0);
        if (result != SQLITE_OK) {
            Logger::begin("Sqlite", Logger::LogLevel_Error) << "SQL Error" << Logger::end();
            return ret;
        }
        int cols = sqlite3_column_count(sql);
        while (result != SQLITE_DONE) {
            result = sqlite3_step(sql);
            if (result == SQLITE_ROW) {
                std::map<std::string, std::string> row;
                for (int i = 0; i < cols; i++) {
                    const char* value = (const char*)sqlite3_column_text(sql, i);
                    row[sqlite3_column_name(sql, i)] = value;
                }
                ret.push_back(row);
            } else if (result != SQLITE_DONE) {
                Logger::begin("Sqlite", Logger::LogLevel_Error) << "Sql Error : " << result << Logger::end();
            }
        }
        return ret;
    }
}