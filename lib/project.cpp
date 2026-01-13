#include "models.hpp"
#include <iostream>

Project::Project(const int& id, const string& name, const int& ownerId) : id(id), name(name), ownerId(ownerId) {}

json Project::toJson() {
    json j;
    j["id"] = id;
    j["name"] = name;
    j["ownerId"] = ownerId;
    j["files"] = json::array();
    for (File& file : files) {
        j["files"].push_back(file.toJson());
    }
    return j;
}

Project Project::fromJson(const json& j) {
    Project project(j.at("id").get<int>(), j.at("name").get<string>(), j.at("ownerId").get<int>());
    for (const auto& fileJson : j.at("files")) {
        project.files.push_back(File::fromJson(fileJson));
    }
    return project;
}

void Project::printInfo() {
    cout << "Project[ID: " << id << ", Name: " << name << ", OwnerID: " << ownerId << "]" << endl;
    cout << "Files:" << endl;
    for (File& file : files) {
        file.printInfo();
    }
}