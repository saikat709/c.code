#pragma once
#include <sqlite3.h>
#include <string>
#include <json.hpp>
using namespace std;

class Database {
    sqlite3* db;
    sqlite3_stmt* stmt;

public:
    Database();
    ~Database();
    bool connect();
    bool executeQuery(const string& query);
    json fetchAsJson(const string& query);
    bool createDbTablesIfNotExists();
    
    void checkMutationQueries();
};
