#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>

class Database {
    sqlite3* db;
    sqlite3_stmt* stmt;

public:
    Database();
    ~Database();
    bool connect();
    bool executeQuery(const std::string& query);
};
