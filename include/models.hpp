#pragma once
#include <string>
#include <json.hpp>
#include <vector>
using namespace std;

class User {
public:
    string username;
    string password;
    int id;

    User(const int& id, const string& uname, const string& pwd); 
    int getId() const { return id; }
    string getUsername() const { return username; }
    json toJson();
    static User fromJson(const json& j);
    void printInfo();
};



class File {
public:
    int id;
    string name;
    string content;
    int projectId;
    int currentEditorId;

    File(const int& id, const string& name, const string& content, const int& projectId);

    json toJson();
    static File fromJson(const json& j);
    void printInfo();
    void changeCurrentEditorId(int id);
};



class Project {
public:
    int id;
    string name;
    int ownerId;
    vector<File> files;

    Project(const int& id, const string& name, const int& ownerId);
    
    json toJson();
    static Project fromJson(const json& j);
    void printInfo();
};