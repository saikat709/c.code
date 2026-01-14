#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;

string parse_code(const string& code, const string& language) {
    string compiler;
    string ext;
    if (language == "c") {
        compiler = "gcc";
        ext = ".c";
    } else if (language == "c++" || language == "cpp") {
        compiler = "g++";
        ext = ".cpp";
    } else {
        return "Invalid language specified. Use 'c' or 'c++'/'cpp'.";
    }

    char temp_filename[L_tmpnam];
    if (tmpnam(temp_filename) == nullptr) {
        return "Error creating temporary file name.";
    }
    string filename = string(temp_filename) + ext;

    ofstream file(filename);
    if (!file) {
        return "Error opening temporary file for writing.";
    }
    file << code;
    file.close();

    string command = compiler + " -fsyntax-only " + filename + " 2>&1";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        remove(filename.c_str());
        return "Error executing compiler.";
    }

    string output;
    vector<char> buffer(1024);
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }

    int return_code = pclose(pipe);
    remove(filename.c_str());
    
    if (return_code == 0) {
        return "Syntax is okay";
    } else {
        return "Syntax error:\n" + output;
    }
}

int main() {
    string c_code = R"(
int main() {
    printf("Hello, world!");  // Missing include
    // return 0;
}
)";

    cout << parse_code(c_code, "c") << endl;
    return 0;
}