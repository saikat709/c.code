#pragma once
#pragma once
#include <string>
#include <vector>
using namespace std;

class Huffman {
public:
    static string compress(const string& input);
    static string decompress(const string& compressedData);
};