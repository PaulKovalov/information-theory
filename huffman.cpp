#include <algorithm>
#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>

using namespace std;

int powers_of_two[] = {1, 2, 4, 8, 16, 32, 64, 128};

struct Tree {
    Tree* left;
    Tree* right;
    string data;
    int freq;
    Tree(string d, int freq = 0, Tree* l = nullptr, Tree* r = nullptr) : data(d), freq(freq), left(l), right(r) {}
};

void traverse(Tree* root, string path, unordered_map<string, string>& m) {
    if (root->left == nullptr && root->right == nullptr) {
        // leaf, add to map
        m[root->data] = path;
    }
    // otherwise, call recursively for left and right nodes
    if (root->left) {
        traverse(root->left, path + "0", m);
    }
    if (root->right) {
        traverse(root->right, path + "1", m);
    }
    delete root;
}

void insert(Tree*& root, string& data, string& path, int i) {
    if (root == nullptr) {
        root = new Tree("");
    }
    if (i == path.size()) {
        // insert
        root->data = data;
        return;
    }
    if (path[i] == '1') {
        // go right
        insert(root->right, data, path, i + 1);
    } else {
        // go left
        insert(root->left, data, path, i + 1);
    }
}

char bits_to_char(string& s, int start) {
    char r = 0;
    int to = min((int)s.size(), start + 8);
    for (int i = start; i < to; ++i) {
        r += (s[i] == '1' ? powers_of_two[i - start] : 0);
    }
    return r;
}

string char_to_bits(char& c) {
    int r = c;
    if (r < 0) {
        r += 256;
    }
    // char encodes 8 bits
    string ans;
    for (int i = 0; i < 8; ++i) {
        ans += ('0' + (r % 2));
        r /= 2;
    }
    return ans;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: ./huffman <e/d> <num of symbols> <path to file to encode>" << endl;
        return 0;
    }
    std::ios::sync_with_stdio(false);
    // parse command line args
    string key = argv[1];
    int n = atoi(argv[2]);
    string filename = argv[3];
    // read file which needs to be processed
    ifstream fin(filename);
    fin.tie(nullptr);
    if (!fin.is_open()) {
        cout << "Error opening file" << endl;
        return 0;
    }
    // get length of file:
    fin.seekg(0, fin.end);
    int f_length = fin.tellg();
    fin.seekg(0, fin.beg);
    char* f_data = new char[f_length];  // file data
    // read file
    fin.read(f_data, f_length);
    // encode mode
    if (key == "e" || key == "E") {
        // copy data to chunks of size n to an unordered_map which automatically counts frequencies of data
        unordered_map<string, int> data;
        data.reserve(f_length / n);
        for (int i = 0; i < f_length; i += n) {
            char subbuff[n + 1];
            memcpy(subbuff, &f_data[i], n);
            subbuff[n] = '\0';
            data[string(subbuff)]++;
        }
        // comparator for priority queue that compares frequencies
        auto q_cmp = [](Tree* l, Tree* r) { return l->freq > r->freq; };
        // create a priority queue for storing nodes
        priority_queue<Tree*, vector<Tree*>, decltype(q_cmp)> pq(q_cmp);
        // insert all nodes stored in unordered map to the priority queue, so they are now sorted
        for (auto it = data.begin(); it != data.end(); ++it) {
            pq.push(new Tree(it->first, it->second));
        }
        // create dummy root
        Tree* root;
        while (!pq.empty()) {
            // take 2 elements from priority queue
            Tree* left = pq.top();
            pq.pop();
            if (!pq.size()) {
                // left is a root node of tree, tree is constructed
                root = left;
                break;
            }
            Tree* right = pq.top();
            pq.pop();
            // add this root to the pq
            pq.push(new Tree(move(left->data + right->data), left->freq + right->freq, left, right));
        }
        // traverse the tree
        unordered_map<string, string> mapping;
        traverse(root, "", mapping);
        // encode table
        string encoded_table = "";
        // using obtained mapping, encode input
        string encoded = "";
        encoded.reserve(f_length);
        for (int i = 0; i < f_length; i += n) {
            // read n chars of file content,
            char subbuff[n + 1];
            memcpy(subbuff, &f_data[i], n);
            subbuff[n] = '\0';
            string to_encode = string(subbuff);
            encoded += mapping[to_encode];
        }
        // open file where write encoded result
        ofstream out("enc.txt");
        out.tie(nullptr);
        // write table size
        out << mapping.size() << endl;
        // write table
        for (auto it = mapping.begin(); it != mapping.end(); ++it) {
            out << it->first << " " << it->second << (next(it) == mapping.end() ? "" : " ");
        }
        out << endl;
        // for every 8 bits of encoded string, construct a char
        string reduced = "";
        for (int i = 0; i < encoded.size(); i += 8) {
            reduced += bits_to_char(encoded, i);
        }
        out << reduced;
    } else if (key == "d" || key == "D") {
        // get size of the table
        int tsize, i = 0, read = 0;
        string stsize = "";
        while (f_data[i] != '\n')
            stsize += f_data[i++];
        tsize = stoi(stsize);
        Tree* root = new Tree("");
        // read tsize pairs starting with n chars and build tree
        ++i;  // skip newline
        while (true) {
            string val, key;
            // read n chars
            char subbuff[n + 1];
            memcpy(subbuff, &f_data[i], n);
            subbuff[n] = '\0';
            val = string(subbuff);
            // read while char is either 0 or 1
            int j = n + i + 1;
            while (f_data[j] == '1' || f_data[j] == '0')
                key += f_data[j++];
            i = j + 1;
            insert(root, val, key, 0);
            if (++read == tsize)
                break;
        }
        // tree is built, read encoded message and convert it to 0-1 string
        string enc = "";
        enc.reserve((f_length - i + 1) * 8);
        for (i; i < f_length; ++i) {
            enc += char_to_bits(f_data[i]);
        }
        ofstream out("dec.txt");
        out.tie(nullptr);
        // iterate over encoded string and decode
        Tree* r = root;
        for (int j = 0; j < enc.size(); ++j) {
            if (r->data != "") {
                // some data here, print it
                out << r->data;
                r = root;
            }
            if (enc[j] == '1') {
                if (r->right)
                    r = r->right;
                else
                    break;
            } else {
                if (r->left)
                    r = r->left;
                else
                    break;
            }
        }

    } else {
        cout << "invlid parameter key: " << key << endl;
    }
    return 0;
}
