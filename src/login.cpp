#include "login.hpp"

using namespace std;

namespace login {
    bool authenticate(const string& username, const string& password) {
        return !username.empty() && !password.empty();
    }
}