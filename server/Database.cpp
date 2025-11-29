#include "Database.hpp"

using namespace std;

Database::Database() : db(nullptr), stmt(nullptr) {}

Database::~Database() {
    if (stmt) sqlite3_finalize(stmt);
    if (db) sqlite3_close(db);
}

bool Database::connect() {
    int rc = sqlite3_open("database.db", &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    cout << "Opened database successfully" << endl;
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
