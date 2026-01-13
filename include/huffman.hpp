#pragma once
#include <string>
#include <json.hpp>
#include <unordered_map>
using namespace std;

class Huffman {
public:
    static string compress(const string& data);
    static string decompress(const string& compressedData, const unordered_map<string, char>& huffmanCode);
};