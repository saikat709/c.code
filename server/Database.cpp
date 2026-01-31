#include "Database.hpp"
#include <json.hpp>
#include <iostream>
#include "models.hpp"

using namespace std;

Database::Database() : db(nullptr), stmt(nullptr) {}

Database::~Database() {
    if (stmt) sqlite3_finalize(stmt);
    if (db) sqlite3_close(db);
}


bool Database::createDbTablesIfNotExists() {
    string userSql = "CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "username TEXT NOT NULL UNIQUE,"
                    "password TEXT NOT NULL);";
    bool userTableCreated = executeQuery(userSql);
    if (!userTableCreated) {
        cerr << "Failed to create users table" << endl;
        return false;
    }

    string projectSql = "CREATE TABLE IF NOT EXISTS projects ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "name TEXT NOT NULL,"
                        "ownerId INTEGER NOT NULL,"
                        "accessKey INTEGER,"
                        "FOREIGN KEY(ownerId) REFERENCES users(id));";
    bool projectTableCreated = executeQuery(projectSql);
    if (!projectTableCreated) {
        cerr << "Failed to create projects table" << endl;
        return false;
    }

    string fileSql = "CREATE TABLE IF NOT EXISTS files ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL,"
                    "content TEXT,"
                    "projectId INTEGER NOT NULL,"
                    "FOREIGN KEY(projectId) REFERENCES projects(id));";
    bool fileTableCreated = executeQuery(fileSql);  
    if (!fileTableCreated) {
        cerr << "Failed to create files table" << endl;
        return false;
    }

    string messageTableCreatedSql = "CREATE TABLE IF NOT EXISTS messages ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "projectId INTEGER NOT NULL,"
                        "sender TEXT NOT NULL,"
                        "message TEXT NOT NULL,"
                        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                        "FOREIGN KEY(projectId) REFERENCES projects(id));";
    bool messageTableCreated = executeQuery(messageTableCreatedSql);
    if (!messageTableCreated) {
        cerr << "Failed to create messages table" << endl;
        return false;
    }

    string membersSql = "CREATE TABLE IF NOT EXISTS project_members ("
                        "userId INTEGER NOT NULL,"
                        "projectId INTEGER NOT NULL,"
                        "PRIMARY KEY(userId, projectId),"
                        "FOREIGN KEY(userId) REFERENCES users(id),"
                        "FOREIGN KEY(projectId) REFERENCES projects(id));";
    if (!executeQuery(membersSql)) {
        cerr << "Failed to create project_members table" << endl;
        return false;
    }

    return true;
}



void Database::checkMutationQueries() {
    json userWithId1 = fetchAsJson("SELECT * FROM users WHERE id = 1;");
    if (userWithId1.empty()) {
        cout << "No user with ID 1 found." << endl;
    } else {
        cout << "User with ID 1 exists." << endl;
        User user = User::fromJson(userWithId1[0]);
        user.printInfo();
    }

    string createUserSql = "INSERT INTO users (username, password) VALUES ('testuser', 'testpass');";
    if (executeQuery(createUserSql)) {
        cout << "Inserted test user successfully." << endl;
    } else {
        cerr << "Failed to insert test user." << endl;
    }
}


bool Database::connect() {
    int rc = sqlite3_open("database.db", &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    cout << "Opened database successfully" << endl;
    if (!createDbTablesIfNotExists()) {
        cerr << "Failed to create database tables" << endl;
        return false;
    }
    cout << "Database tables ensured" << endl;

    // checkMutationQueries();
    return true;
}

bool Database::executeQuery(const string& query) {
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, query.c_str(), 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
}


json Database::fetchAsJson(const string& query) {
    json result = json::array();
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return result;
    }

    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        json row;
        for (int i = 0; i < cols; i++) {
            string colName = sqlite3_column_name(stmt, i);
            switch (sqlite3_column_type(stmt, i)) {
                case SQLITE_INTEGER:
                    row[colName] = sqlite3_column_int(stmt, i);
                    break;
                case SQLITE_FLOAT:
                    row[colName] = sqlite3_column_double(stmt, i);
                    break;
                case SQLITE_TEXT:
                    row[colName] = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                    break;
                case SQLITE_NULL:
                    row[colName] = nullptr;
                    break;
                default:
                    row[colName] = nullptr;
            }
        }
        result.push_back(row);
    }

    sqlite3_finalize(stmt);
    return result;
}