#include "Database.hpp"

namespace Engine {
    bool hasInited = false;
    
    Database::Database(std::string database) {
        if (!hasInited) {
            Logger::begin("Sqlite", Logger::LogLevel_Verbose) << "Initalise SQLITE" << Logger::end();
            sqlite3_initialize();
            hasInited = true;
        }
        int result = sqlite3_open_v2(database.c_str(), &this->_database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if (result != SQLITE_OK) {
            Logger::begin("Sqlite", Logger::LogLevel_Error) << "Failed opening database: " << sqlite3_errmsg(this->_database) << Logger::end();
        }
    }
    
    Database::~Database() {
        sqlite3_close(this->_database);
    }
    
    bool Database::Execute(std::string statement) {
        char* errmsg = 0;
        int result = sqlite3_exec(this->_database, statement.c_str(), NULL, NULL, &errmsg);
        if (result != SQLITE_OK) {
            Logger::begin("Sqlite", Logger::LogLevel_Error) << "SQL Error : " << errmsg << Logger::end();
            return false;
        }
        return true;
    }
    
    std::vector< std::map<std::string, std::string> > Database::ExecutePrepare(std::string statement) {
        std::vector< std::map<std::string, std::string> > ret;
        sqlite3_stmt *sql;
        int result = sqlite3_prepare_v2(this->_database, statement.c_str(), -1, &sql, 0);
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