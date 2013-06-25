#pragma once

#include <vector>
#include <map>

#include "Logger.hpp"

#ifdef _PLATFORM_WIN32
// TODO: Fix this
#include "../../../org/sqlite-amalgamation-3071700/sqlite3.h"
#else
#include <sqlite3.h>
#endif

namespace Engine {
    class Database {
    public:
        Database(std::string database);
        ~Database();
        
        bool Execute(std::string statement);
        std::vector< std::map<std::string, std::string> > ExecutePrepare(std::string statement);

    private:
        sqlite3 *_database;
    };
}