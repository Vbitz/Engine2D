#pragma once

#include <vector>
#include <map>

#include "Logger.hpp"

// TODO: use extern/sqlite3 insteed
#include <sqlite3.h>

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