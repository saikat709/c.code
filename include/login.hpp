#pragma once

#include <string>

namespace login {
    bool authenticate(const std::string& username, const std::string& password);
}