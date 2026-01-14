#pragma once
#include <string>
#include <json.hpp>
#include "Database.hpp"
#include "models.hpp"
using namespace std;

class DBActions {
public:
    static bool loginUser(Database& db, const string& username, const string& password);
    static User getUserByUsername(Database& db, const string& username);
    static bool createProject(Database& db, const std::string& projectName, const std::string& ownerUsername);
    static bool createUser(Database& db, const std::string& username, const std::string& password, const std::string& email);
};