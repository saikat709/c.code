#include <actions.hpp>
#include <Database.hpp>
#include <models.hpp>
using namespace std;

bool DBActions::loginUser(Database& db, const string& username, const string& password) {
    json queryResult = db.fetchAsJson("SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "';");
    return !queryResult.empty();
}


User DBActions::getUserByUsername(Database& db, const string& username) {
    json queryResult = db.fetchAsJson("SELECT * FROM users WHERE username = '" + username + "';");
    if (!queryResult.empty()) {
        int id = queryResult[0]["id"];
        string username = queryResult[0]["username"];
        return User(id, username, "");
    }
    return User(0, "", "");
}


bool DBActions::createUser(Database& db, const string& username, const string& password) {
    string query = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "');";
    return db.executeQuery(query);
}


bool DBActions::createProject(Database& db, const string& projectName, int ownerId) {
    // Generate access key: sequential from 10
    string keyQuery = "SELECT COALESCE(MAX(accessKey), 9) + 1 as nextKey FROM projects;";
    json keyResult = db.fetchAsJson(keyQuery);
    int accessKey = keyResult[0]["nextKey"];

    string query = "INSERT INTO projects (name, ownerId, accessKey) VALUES ('" + projectName + "', " + to_string(ownerId) + ", " + to_string(accessKey) + ");";
    return db.executeQuery(query);
}

json DBActions::getProjects(Database& db, int ownerId) {
    // Get projects owned OR projects user is a member of
    string query = "SELECT p.*, (SELECT COUNT(*) FROM files f WHERE f.projectId = p.id) as fileCount "
                   "FROM projects p "
                   "WHERE p.ownerId = " + to_string(ownerId) + " "
                   "OR p.id IN (SELECT projectId FROM project_members WHERE userId = " + to_string(ownerId) + ");";
    return db.fetchAsJson(query);
}

int DBActions::createFile(Database& db, const string& fileName, int projectId) {
    string query = "INSERT INTO files (name, content, projectId) VALUES ('" + fileName + "', '', " + to_string(projectId) + ");";
    if (db.executeQuery(query)) {
        json res = db.fetchAsJson("SELECT last_insert_rowid() as id;");
        if (!res.empty()) return res[0]["id"];
    }
    return -1;
}

json DBActions::getFiles(Database& db, int projectId) {
    string query = "SELECT id, name FROM files WHERE projectId = " + to_string(projectId) + ";";
    return db.fetchAsJson(query);
}

bool DBActions::updateFileContent(Database& db, int fileId, const string& content) {
    // Need to escape single quotes in content for SQL
    string escapedContent = content;
    size_t pos = 0;
    while ((pos = escapedContent.find("'", pos)) != string::npos) {
        escapedContent.replace(pos, 1, "''");
        pos += 2;
    }
    string query = "UPDATE files SET content = '" + escapedContent + "' WHERE id = " + to_string(fileId) + ";";
    return db.executeQuery(query);
}

string DBActions::getFileContent(Database& db, int fileId) {
    string query = "SELECT content FROM files WHERE id = " + to_string(fileId) + ";";
    json result = db.fetchAsJson(query);
    if (!result.empty() && result[0].contains("content") && !result[0]["content"].is_null()) {
        return result[0]["content"];
    }
    return "";
}

bool DBActions::sendMessage(Database& db, int projectId, const string& sender, const string& message) {
    string escapedMsg = message;
    size_t pos = 0;
    while ((pos = escapedMsg.find("'", pos)) != string::npos) {
        escapedMsg.replace(pos, 1, "''");
        pos += 2;
    }
    string query = "INSERT INTO messages (projectId, sender, message) VALUES (" + to_string(projectId) + ", '" + sender + "', '" + escapedMsg + "');";
    return db.executeQuery(query);
}

json DBActions::getMessages(Database& db, int projectId) {
    string query = "SELECT sender, message, timestamp FROM messages WHERE projectId = " + to_string(projectId) + " ORDER BY timestamp ASC;";
    return db.fetchAsJson(query);
}

json DBActions::getSharedProjects(Database& db, int userId) {
    string query = "SELECT p.* FROM projects p JOIN project_members m ON p.id = m.projectId WHERE m.userId = " + to_string(userId) + ";";
    return db.fetchAsJson(query);
}

bool DBActions::joinProject(Database& db, int userId, int projectId, int accessKey) {
    // Verify project exists and key matches
    string checkQuery = "SELECT * FROM projects WHERE id = " + to_string(projectId) + " AND accessKey = " + to_string(accessKey) + ";";
    json check = db.fetchAsJson(checkQuery);
    if (check.empty()) return false;

    // Add to members
    string query = "INSERT OR IGNORE INTO project_members (userId, projectId) VALUES (" + to_string(userId) + ", " + to_string(projectId) + ");";
    return db.executeQuery(query);
}

json DBActions::getProjectAccessInfo(Database& db, int projectId) {
    string query = "SELECT id, accessKey FROM projects WHERE id = " + to_string(projectId) + ";";
    json result = db.fetchAsJson(query);
    if (!result.empty()) return result[0];
    return json::object();
}