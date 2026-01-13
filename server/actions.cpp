#include <actions.hpp>
#include <Database.hpp>
using namespace std;

bool DBActions::loginUser(Database& db, const string& username, const string& password) {
    json queryResult = db.fetchAsJson("SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "';");
    return !queryResult.empty();
}


bool DBActions::createUser(Database& db, const string& username, const string& password, const string& email) {
    string query = "INSERT INTO users (username, password, email) VALUES ('" + username + "', '" + password + "', '" + email + "');";
    return db.executeQuery(query);
}


bool DBActions::createProject(Database& db, const string& projectName, const string& ownerUsername) {
    string query = "INSERT INTO projects (name, owner) VALUES ('" + projectName + "', '" + ownerUsername + "');";
    return db.executeQuery(query);
}