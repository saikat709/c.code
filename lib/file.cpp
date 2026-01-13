#include "models.hpp"
#include <iostream>

File::File(const int& id, const string& name, const string& content, const int& projectId) 
    : id(id), name(name), content(content), projectId(projectId) {}

void File::changeCurrentEditorId(int editorId) {
    currentEditorId = editorId;
}   

json File::toJson() {
    return json{
        {"id", id}, 
        {"name", name}, 
        {"content", content}, 
        {"projectId", projectId},
        {"currentEditorId", currentEditorId}
    };
}

File File::fromJson(const json& j) {
    File file = File(j.at("id").get<int>(), j.at("name").get<string>(), j.at("content").get<string>(), j.at("projectId").get<int>());
    file.currentEditorId = j.at("currentEditorId").get<int>();
    return file;
}

void File::printInfo() {
    cout << "File[ID: " << id << ", Name: " << name << ", ProjectID: " << projectId << "]" << endl;
}