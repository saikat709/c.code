#include "login.hpp"

namespace login {
    bool authenticate(const string& username, const string& password) {
        return !username.empty() && !password.empty();
    }
}