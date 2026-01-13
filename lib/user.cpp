#include <iostream>
#include "models.hpp"
using namespace std;


User::User(const int& id, const string& uname, const string& pwd) : id(id), username(uname), password(pwd) {}

json User::toJson() {
    return json{{"id", id}, {"username", username}, {"password", password}};
}

User User::fromJson(const json& j) {
    return User(j.at("id").get<int>(), j.at("username").get<string>(), j.at("password").get<string>());
}

void User::printInfo() {
    cout << "User[ID: " << id << ", Username: " << username << "]" << endl;
}