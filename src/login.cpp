#include "login.hpp"
#include <iostream>

using namespace std;

namespace login {
    bool authenticate(const string& username, const string& password) {
        cout << "Authenticating user: " << username << endl;
        cout << "Password: " << password << endl;
        return !username.empty() && !password.empty();
    }
}