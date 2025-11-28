#include "login.hpp"

namespace login {
    bool authenticate(const std::string& username, const std::string& password) {
        return !username.empty() && !password.empty();
    }
}