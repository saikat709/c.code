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
    static bool createProject(Database& db, const std::string& projectName, int ownerId);
    static bool createUser(Database& db, const std::string& username, const std::string& password);
    
    // Project & File actions
    static json getProjects(Database& db, int ownerId);
    static int createFile(Database& db, const std::string& fileName, int projectId);
    static json getFiles(Database& db, int projectId);
    static bool updateFileContent(Database& db, int fileId, const std::string& content);
    static std::string getFileContent(Database& db, int fileId);

    // Message actions
    static bool sendMessage(Database& db, int projectId, const std::string& sender, const std::string& message);
    static json getMessages(Database& db, int projectId);

    // Collaboration
    static json getSharedProjects(Database& db, int userId);
    static bool joinProject(Database& db, int userId, int projectId, int accessKey);
    static json getProjectAccessInfo(Database& db, int projectId);
};