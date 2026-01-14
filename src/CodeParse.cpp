#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "CodeParse.hpp"
#include "json.hpp"
using namespace std;


json CodeParse::parseCode(const string& code, const string& language) {
    string compiler;
    string ext;
    if (language == "c") {
        compiler = "gcc";
        ext = ".c";
    } else if (language == "c++" || language == "cpp") {
        compiler = "g++";
        ext = ".cpp";
    } else {
        return json{
            {"status", false},
            {"error", "Invalid language specified. Use 'c' or 'c++'/'cpp'."}
        };
    }
    
    char temp_filename[L_tmpnam];
    if (tmpnam(temp_filename) == nullptr) {
        return json{
            {"status", false},
            {"error", "Error creating temporary file name."}
        };
    }
    string filename = string(temp_filename) + ext;

    ofstream file(filename);
    if (!file) {
        return json{
            {"status", false},
            {"error", "Error opening temporary file for writing."}
        };
    }
    file << code;
    file.close();

    string command = compiler + " -fsyntax-only " + filename + " 2>&1";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        remove(filename.c_str());
        return json{
            {"status", false},
            {"error", "Error executing compiler."}
        };
    }

    string output;
    vector<char> buffer(1024);
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }

    int return_code = pclose(pipe);

    remove(filename.c_str());
    if (return_code == 0) {
        return json{
            {"status", true},
            {"message", "Syntax is okay"}
        };
    } else {
        return json{
            {"status", false},
            {"error", "Syntax error:\n" + output}
        };
    }
}