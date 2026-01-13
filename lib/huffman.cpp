#include "huffman.hpp" 
#include <queue>
#include <unordered_map>
#include <string>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char character, int frequency) : ch(character), freq(frequency), left(nullptr), right(nullptr) {}
};

struct compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

void buildCodes(Node* root, const string& str, unordered_map<char, string>& huffmanCode) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    buildCodes(root->left, str + "0", huffmanCode);
    buildCodes(root->right, str + "1", huffmanCode);
}

string Huffman::compress(const string& data) {
    unordered_map<char, int> freq;
    for (char ch : data) {
        freq[ch]++;
    }

    priority_queue<Node*, vector<Node*>, compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();

        Node *newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    Node* root = pq.top();
    unordered_map<char, string> huffmanCode;
    buildCodes(root, "", huffmanCode);

    string compressedData = "";
    for (char ch : data) {
        compressedData += huffmanCode[ch];
    }

    unordered_map<string, char> reverseCodeMap;
    for (auto& pair : huffmanCode) {
        reverseCodeMap[pair.second] = pair.first;
    }

    return compressedData;
}

string Huffman::decompress(const string& compressedData, const unordered_map<string, char>& reverseCodeMap) {
    string result = "";
    string current = "";
    for (char bit : compressedData) {
        current += bit;
        if (reverseCodeMap.count(current)) {
            result += reverseCodeMap.at(current);
            current = "";
        }
    }
    return result;
}