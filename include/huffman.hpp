#pragma once
#pragma once
#include <string>
#include <vector>
using namespace std;


struct Node {
    char ch;
    int freq;
    Node *left, *right;
    
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};


class Huffman {
public:
    string compress(const string& input);
    string decompress(const string& compressed);

    Huffman();
    ~Huffman();

private:
    Node* root;
    unordered_map<char, string> huffmanCodes;
    unordered_map<char, int> freqMap;

    void buildCodes(Node* node, string code);
    Node* buildTree();
};