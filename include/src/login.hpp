#pragma once

#include <string>

using namespace std;

namespace login {
    bool authenticate(const string& username, const string& password);
}