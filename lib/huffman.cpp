#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>
#include <sstream>
#include "huffman.hpp"
using namespace std;


string encodeTree(Node* node) {
    if (!node) return "";
    
    if (!node->left && !node->right) {
        return "1" + string(1, node->ch);
    }
    
    return "0" + encodeTree(node->left) + encodeTree(node->right);
}

Node* decodeTree(const string& encoded, int& idx) {
    if (idx >= encoded.length()) return nullptr;
    
    if (encoded[idx] == '1') {
        idx++;
        char ch = encoded[idx];
        idx++;
        return new Node(ch, 0);
    }
    
    idx++;
    Node* node = new Node('\0', 0);
    node->left = decodeTree(encoded, idx);
    node->right = decodeTree(encoded, idx);
    
    return node;
}



Huffman::Huffman() : root(nullptr) {}
Huffman::~Huffman() {
    root = nullptr;
}

void Huffman::buildCodes(Node* node, string code) {
    if (!node) return;
    
    if (!node->left && !node->right) {
        huffmanCodes[node->ch] = code.empty() ? "0" : code;
        return;
    }
    
    buildCodes(node->left, code + "0");
    buildCodes(node->right, code + "1");
}

Node* Huffman::buildTree() {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    
    for (auto& p : freqMap) {
        pq.push(new Node(p.first, p.second));
    }
    
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        
        Node* merged = new Node('\0', left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        
        pq.push(merged);
    }
    
    return pq.top();
}

string Huffman::compress(const string& input) {
    if (input.empty()) return "";
    
    freqMap.clear();
    huffmanCodes.clear();
    
    for (char ch : input) {
        freqMap[ch]++;
    }
    
    root = buildTree();
    buildCodes(root, "");
    
    string encoded = "";
    for (char ch : input) {
        encoded += huffmanCodes[ch];
    }
    
    string treeEncoded = encodeTree(root);
    
    string result = treeEncoded + "|" + encoded;
    
    return result;
}

string Huffman::decompress(const string& compressed) {
    if (compressed.empty()) return "";
    
    size_t separator = compressed.find('|');
    if (separator == string::npos) return "";
    
    string treeEncoded = compressed.substr(0, separator);
    string encoded = compressed.substr(separator + 1);
    
    int idx = 0;
    root = decodeTree(treeEncoded, idx);
    
    string decoded = "";
    Node* curr = root;
    
    for (char bit : encoded) {
        if (bit == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
        
        if (!curr->left && !curr->right) {
            decoded += curr->ch;
            curr = root;
        }
    }
    
    return decoded;
};