#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <cstring>
#include <memory>
#include <functional>
#include <iomanip>

using namespace std;

// ============== Task 3: Bitmap Class ==============
class Bitmap {
private:
    unsigned char* M;
    int N;
    int _sz;

    void expand(int k) {
        if (k < 8 * N) return;
        int newN = max(N * 2, (k + 7) / 8 + 1);
        unsigned char* newM = new unsigned char[newN];
        memset(newM, 0, newN);
        if (M) {
            memcpy(newM, M, N);
            delete[] M;
        }
        M = newM;
        N = newN;
    }

public:
    Bitmap(int n = 8) {
        N = (n + 7) / 8;
        M = new unsigned char[N];
        memset(M, 0, N);
        _sz = 0;
    }
    
    ~Bitmap() {
        delete[] M;
        M = nullptr;
    }
    
    void set(int k) {
        expand(k);
        if (k >= _sz) _sz = k + 1;
        M[k >> 3] |= (0x80 >> (k & 0x07));
    }
    
    bool test(int k) const {
        if (k >= _sz) return false;
        return M[k >> 3] & (0x80 >> (k & 0x07));
    }
    
    string bits2string(int n) const {
        if (n <= 0) return "";
        string s(n, '0');
        for (int i = 0; i < n && i < _sz; i++) {
            if (test(i)) s[i] = '1';
        }
        return s;
    }
    
    int getLength() const { return _sz; }
    
    void appendBit(bool bit) {
        if (bit) {
            set(_sz);
        } else {
            expand(_sz);
            _sz++;
        }
    }
};

// ============== Task 3: HuffCode Class ==============
class HuffCode {
private:
    Bitmap code;
    
public:
    HuffCode() : code(128) {}
    
    void append(bool bit) {
        code.appendBit(bit);
    }
    
    int length() const {
        return code.getLength();
    }
    
    string toString() const {
        return code.bits2string(length());
    }
    
    bool getBit(int pos) const {
        return code.test(pos);
    }
};

// ============== Task 1: Binary Tree Data Structure ==============
struct BinNode {
    char data;
    int weight;
    shared_ptr<BinNode> left;
    shared_ptr<BinNode> right;
    
    BinNode(char d = '\0', int w = 0) : data(d), weight(w), left(nullptr), right(nullptr) {}
    
    bool isLeaf() const {
        return !left && !right;
    }
};

class BinTree {
private:
    shared_ptr<BinNode> root;
    
public:
    BinTree() : root(nullptr) {}
    
    BinTree(shared_ptr<BinNode> r) : root(r) {}
    
    bool empty() const { return root == nullptr; }
    
    shared_ptr<BinNode> getRoot() const { return root; }
    
    void setRoot(shared_ptr<BinNode> r) { root = r; }
    
    void display(shared_ptr<BinNode> node, int depth = 0) const {
        if (!node) return;
        
        display(node->right, depth + 1);
        
        for (int i = 0; i < depth; i++) cout << "    ";
        if (node->data == '\0')
            cout << "[" << node->weight << "]" << endl;
        else
            cout << "'" << node->data << "'(" << node->weight << ")" << endl;
        
        display(node->left, depth + 1);
    }
    
    void show() {
        cout << "Binary Tree Structure:" << endl;
        display(root);
        cout << endl;
    }
};

// ============== Task 2: Huffman Tree ==============
class HuffTree {
private:
    BinTree tree;
    map<char, HuffCode> codeTable;
    map<char, int> freqTable;
    
    struct NodeCompare {
        bool operator()(const shared_ptr<BinNode>& a, const shared_ptr<BinNode>& b) const {
            return a->weight > b->weight;
        }
    };
    
    void generateCodes(shared_ptr<BinNode> node, HuffCode currentCode) {
        if (!node) return;
        
        if (node->isLeaf() && node->data != '\0') {
            codeTable[node->data] = currentCode;
            return;
        }
        
        HuffCode leftCode = currentCode;
        leftCode.append(false);
        generateCodes(node->left, leftCode);
        
        HuffCode rightCode = currentCode;
        rightCode.append(true);
        generateCodes(node->right, rightCode);
    }
    
public:
    // ============== Task 4: Huffman Encoding Algorithm ==============
    void build(const string& text) {
        freqTable.clear();
        for (char ch : text) {
            if (isalpha(ch)) {
                char lowerCh = tolower(ch);
                freqTable[lowerCh]++;
            }
        }
        
        cout << "Character frequency counted. " << freqTable.size() << " different letters found." << endl;
        
        if (freqTable.empty()) {
            cout << "Error: No letters in text!" << endl;
            tree.setRoot(nullptr);
            return;
        }
        
        priority_queue<shared_ptr<BinNode>, vector<shared_ptr<BinNode>>, NodeCompare> minHeap;
        
        for (const auto& pair : freqTable) {
            auto node = make_shared<BinNode>(pair.first, pair.second);
            minHeap.push(node);
        }
        
        cout << "\nBuilding Huffman tree..." << endl;
        int step = 1;
        while (minHeap.size() > 1) {
            auto left = minHeap.top(); minHeap.pop();
            auto right = minHeap.top(); minHeap.pop();
            
            auto parent = make_shared<BinNode>('\0', left->weight + right->weight);
            parent->left = left;
            parent->right = right;
            
            minHeap.push(parent);
            step++;
        }
        
        if (!minHeap.empty()) {
            tree.setRoot(minHeap.top());
            cout << "Huffman tree built successfully! Root weight = " << minHeap.top()->weight << endl;
        }
        
        codeTable.clear();
        if (tree.getRoot()) {
            HuffCode startCode;
            generateCodes(tree.getRoot(), startCode);
            cout << "Code table generated!" << endl;
        }
    }
    
    HuffCode getCode(char ch) const {
        char lowerCh = tolower(ch);
        auto it = codeTable.find(lowerCh);
        if (it != codeTable.end()) {
            return it->second;
        }
        return HuffCode();
    }
    
    int getFrequency(char ch) const {
        char lowerCh = tolower(ch);
        auto it = freqTable.find(lowerCh);
        if (it != freqTable.end()) {
            return it->second;
        }
        return 0;
    }
    
    void displayCodeTable() const {
        cout << "\n=== Huffman Code Table ===" << endl;
        cout << "Char\tFreq\tCode\t\tLength" << endl;
        cout << "--------------------------------" << endl;
        
        vector<pair<char, pair<int, HuffCode>>> entries;
        for (const auto& pair : codeTable) {
            entries.push_back({pair.first, {getFrequency(pair.first), pair.second}});
        }
        
        sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            return a.second.first > b.second.first;
        });
        
        for (const auto& entry : entries) {
            cout << "'" << entry.first << "'\t" 
                 << entry.second.first << "\t" 
                 << entry.second.second.toString() << "\t\t"
                 << entry.second.second.length() << " bits" << endl;
        }
    }
    
    void showTree() {
        cout << "\n=== Huffman Tree Structure ===" << endl;
        tree.show();
    }
    
    string encode(const string& text) const {
        string encoded;
        for (char ch : text) {
            if (isalpha(ch)) {
                HuffCode code = getCode(ch);
                encoded += code.toString();
            }
        }
        return encoded;
    }
    
    string decode(const string& encoded) const {
        string decoded;
        shared_ptr<BinNode> current = tree.getRoot();
        
        if (!current) return "";
        
        for (char bit : encoded) {
            if (bit == '0') {
                if (current->left) current = current->left;
                else break;
            } else if (bit == '1') {
                if (current->right) current = current->right;
                else break;
            }
            
            if (current && current->isLeaf() && current->data != '\0') {
                decoded += current->data;
                current = tree.getRoot();
            }
        }
        
        return decoded;
    }
};

// ============== Task 4: Get "I have a dream" speech ==============
string getDreamSpeech() {
    return "I have a dream that one day this nation will rise up and live out the true "
           "meaning of its creed We hold these truths to be selfevident that all men "
           "are created equal I have a dream that one day on the red hills of Georgia "
           "the sons of former slaves and the sons of former slave owners will be able "
           "to sit down together at the table of brotherhood I have a dream that one "
           "day even the state of Mississippi a state sweltering with the heat of "
           "injustice sweltering with the heat of oppression will be transformed into "
           "an oasis of freedom and justice I have a dream that my four little children "
           "will one day live in a nation where they will not be judged by the color of "
           "their skin but by the content of their character I have a dream today";
}

// ============== Task 5: Test word encoding ==============
void testWords(HuffTree& huffTree) {
    cout << "\n=== Word Encoding Test ===" << endl;
    
    vector<string> words = {
        "dream", "freedom", "justice", "nation", 
        "equal", "brotherhood", "character", "today"
    };
    
    cout << "Word\t\tASCII bits\tHuffman Code\t\t\tLength\tCompression" << endl;
    cout << "-------------------------------------------------------------------------" << endl;
    
    for (const string& word : words) {
        string encoded = huffTree.encode(word);
        int asciiBits = word.length() * 8;
        int huffmanBits = 0;
        
        for (char ch : word) {
            huffmanBits += huffTree.getCode(ch).length();
        }
        
        double compressionRatio = 1.0 - (double)huffmanBits / asciiBits;
        
        cout << word;
        if (word.length() < 8) cout << "\t";
        cout << "\t" << asciiBits << "\t\t";
        
        if (encoded.length() > 30) {
            cout << encoded.substr(0, 30) << "...";
        } else {
            cout << encoded;
            for (size_t i = encoded.length(); i < 30; i++) cout << " ";
        }
        
        cout << "\t" << huffmanBits << "\t";
        cout << fixed << setprecision(1) << (compressionRatio * 100) << "%" << endl;
    }
}

// ============== Main Function ==============
int main() {
    cout << "========== Binary Tree and Huffman Coding Experiment ==========\n" << endl;
    
    // Task 4: Read speech text
    string speechText = getDreamSpeech();
    cout << "1. Reading 'I have a dream' speech" << endl;
    cout << "   Text length: " << speechText.length() << " characters" << endl;
    cout << "   First 100 chars: " << speechText.substr(0, 100) << "..." << endl << endl;
    
    // Task 2 & 4: Build Huffman tree
    cout << "2. Building Huffman tree" << endl;
    HuffTree huffTree;
    cout << "DEBUG: Before build()" << endl;
    huffTree.build(speechText);
    cout << "DEBUG: After build()" << endl;  // 看这个是否输出
    huffTree.build(speechText);
    
    // Display code table
    huffTree.displayCodeTable();
    
    // Task 5: Test word encoding
    testWords(huffTree);
    
    // Encode/Decode test
    cout << "\n=== Encode/Decode Test ===" << endl;
    string testPhrase = "ihaveadream";
    string encoded = huffTree.encode(testPhrase);
    string decoded = huffTree.decode(encoded);
    
    cout << "Original phrase: \"" << testPhrase << "\"" << endl;
    cout << "Encoded: " << encoded << endl;
    cout << "Decoded: \"" << decoded << "\"" << endl;
    cout << "Test result: " << (testPhrase == decoded ? "CORRECT" : "WRONG") << endl;
    
    // Compression statistics
    cout << "\n=== Compression Statistics ===" << endl;
    int totalChars = 0;
    int asciiBits = 0;
    int huffmanBits = 0;
    
    for (char ch : speechText) {
        if (isalpha(ch)) {
            totalChars++;
            asciiBits += 8;
            huffmanBits += huffTree.getCode(ch).length();
        }
    }
    
    double compressionRatio = 1.0 - (double)huffmanBits / asciiBits;
    
    cout << "Total letters: " << totalChars << endl;
    cout << "ASCII encoding size: " << asciiBits << " bits (" << (asciiBits/8) << " bytes)" << endl;
    cout << "Huffman encoding size: " << huffmanBits << " bits (" << (huffmanBits/8.0) << " bytes)" << endl;
    cout << "Compression ratio: " << fixed << setprecision(2) << (compressionRatio * 100) << "%" << endl;
    cout << "Average code length: " << fixed << setprecision(2) << (double)huffmanBits / totalChars << " bits/char" << endl;
    
    cout << "\n========== Experiment Completed! ==========" << endl;
    
    // Show tree structure (optional)
    char showTree;
    cout << "\nShow Huffman tree structure? (y/n): ";
    cin >> showTree;
    if (showTree == 'y' || showTree == 'Y') {
        huffTree.showTree();
    }
    
    return 0;
}