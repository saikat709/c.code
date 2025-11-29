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
    // SQLite doesn't have a direct equivalent to MYSQL_RES/ROW in the same way, 
    // we'll simplify for now or just return bool for execution.
    // For fetching, we might need a different approach, but let's keep the interface simple for now.
    // We will remove getResult/fetchRow/freeResult for now as they are specific to the MySQL wrapper style
    // and implement a callback or prepared statement approach later if needed.
    // For this task, we just need basic connection and execution.
};
