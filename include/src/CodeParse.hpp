#pragma once
#include <string>
#include <vector>
#include <json.hpp>
using namespace std;

class CodeParse {
public:
    static json parseCode(const string& code, const string& language);
};