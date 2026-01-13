#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <algorithm>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Cmp {
    bool operator()(Node* a, Node* b) {
        if(a->freq == b->freq)
            return a->ch > b->ch;
        return a->freq > b->freq;
    }
};

Node* buildTree(const unordered_map<char,int>& freq) {
    priority_queue<Node*, vector<Node*>, Cmp> pq;

    for (auto& p : freq) pq.push(new Node(p.first, p.second));

    while (pq.size() > 1) {
        Node* l = pq.top(); pq.pop();
        Node* r = pq.top(); pq.pop();
        Node* n = new Node('\0', l->freq + r->freq);
        n->left = l; n->right = r;
        pq.push(n);
    }
    return pq.top();
}

void buildCodes(Node* n, const string& s, unordered_map<char,string>& codes) {
    if (!n) return;
    if (!n->left && !n->right) {
        codes[n->ch] = s.empty() ? "0" : s;
        return;
    }
    if (n->left) buildCodes(n->left, s + "0", codes);
    if (n->right) buildCodes(n->right, s + "1", codes);
}

vector<uint8_t> packBits(const string& bits, int& bitLen) {
    vector<uint8_t> out; 
    uint8_t byte = 0; 
    int cnt = 0;
    for (char b : bits) {
        byte = (byte << 1) | (b == '1'); 
        cnt++;
        if (cnt == 8) { 
            out.push_back(byte); 
            byte = 0; 
            cnt = 0; 
        }
    }
    if (cnt) { 
        byte <<= (8 - cnt); 
        out.push_back(byte); 
    }
    bitLen = bits.size();
    return out;
}

string unpackBits(const vector<uint8_t>& data, int bitLen) {
    string bits;
    for (uint8_t b : data) {
        for (int i = 7; i >= 0 && bits.size() < (size_t)bitLen; i--)
            bits += ((b >> i) & 1) ? '1' : '0';
    }
    return bits;
}

string decode(Node* root, const string& bits) {
    string out; 
    Node* cur = root;
    for (char b : bits) {
        cur = (b == '0') ? cur->left : cur->right;
        if (!cur->left && !cur->right) { 
            out += cur->ch; 
            cur = root; 
        }
    }
    return out;
}

string buildPayload(const unordered_map<char,int>& freq, int bitLen,
                    const vector<uint8_t>& compressed) {
    string payload;
    auto putInt = [&](int x){ 
        payload.append(reinterpret_cast<const char*>(&x), sizeof(int)); 
    };
    int freqCount = freq.size();
    putInt(freqCount);
    
    // Sort entries to ensure deterministic order
    vector<pair<char, int>> sortedFreq(freq.begin(), freq.end());
    sort(sortedFreq.begin(), sortedFreq.end());
    
    for (auto& p : sortedFreq) { 
        payload.push_back(p.first); 
        putInt(p.second); 
    }
    putInt(bitLen);
    putInt(compressed.size());
    payload.append(reinterpret_cast<const char*>(compressed.data()), compressed.size());
    return payload;
}

void parsePayload(
    const string& payload,
    unordered_map<char,int>& freq,
    int& bitLen,
    vector<uint8_t>& compressed
) {
    size_t idx = 0;
    auto getInt = [&](int& x){ 
        memcpy(&x, payload.data() + idx, sizeof(int)); 
        idx += sizeof(int); 
    };
    int freqCount; 
    getInt(freqCount);
    for (int i = 0; i < freqCount; i++) {
        char c = payload[idx++];
        int f; 
        getInt(f);
        freq[c] = f;
    }
    getInt(bitLen);
    int byteCount; 
    getInt(byteCount);
    compressed = vector<uint8_t>(payload.begin() + idx, payload.begin() + idx + byteCount);
}

// Compress function
string compress(const string& input) {
    // Build frequency table
    unordered_map<char,int> freq;
    for (char c : input) freq[c]++;

    // Build Huffman tree & codes
    Node* root = buildTree(freq);
    unordered_map<char,string> codes;
    buildCodes(root, "", codes);

    // Encode input
    string bits;
    for (char c : input) bits += codes[c];
    int bitLen;
    vector<uint8_t> compressed = packBits(bits, bitLen);

    // Build payload string (ready to send)
    return buildPayload(freq, bitLen, compressed);
}

// Decompress function
string decompress(const string& compressedData) {
    // Parse payload
    unordered_map<char,int> freq;
    int bitLen;
    vector<uint8_t> compressed;
    parsePayload(compressedData, freq, bitLen, compressed);

    // Rebuild tree and decode
    Node* root = buildTree(freq);
    string unpackedBits = unpackBits(compressed, bitLen);
    return decode(root, unpackedBits);
}

int main() {
    string input = "{\"action\":\"login\",\"password\":\"\",\"username\":\"\"}";
    
    cout << "=== Huffman Compression Test ===" << endl;
    cout << "Original message: " << input << endl;
    cout << "Original size: " << input.size() << " bytes" << endl;
    cout << endl;

    // Compress
    string compressed = compress(input);
    cout << "Compressed data (hex): ";
    for (size_t i = 0; i < min(compressed.size(), (size_t)50); i++) {
        printf("%02x ", (unsigned char)compressed[i]);
    }
    if (compressed.size() > 50) cout << "...";
    cout << endl;
    cout << "Compressed size: " << compressed.size() << " bytes" << endl;
    cout << "Compression ratio: " << (100.0 * compressed.size() / input.size()) << "%" << endl;
    cout << endl;

    // Decompress
    string decompressed = decompress(compressed);
    cout << "Decompressed message: " << decompressed << endl;
    cout << endl;

    // Verify
    if (input == decompressed) {
        cout << "✓ SUCCESS: Decompressed message matches original!" << endl;
    } else {
        cout << "✗ ERROR: Decompressed message does NOT match original!" << endl;
    }

    return 0;
}