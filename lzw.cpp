#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

int NUM_OF_CHARS = 128;  // consider all ASCII characters
int powers_of_two[] = {1, 2, 4, 8, 16, 32, 64, 128};

/**
 * Separate table class to encapsulate implementation.
 **/
class Table {
    struct Trie {
        // better to use unique_ptr, but performance may degrade
        // TODO research this
        unordered_map<char, Trie*> data;
        int v;
        Trie(int v = -1) : v(v){};
    };
    Trie* root;
    int size = 0;
    int CHAR_MIN = static_cast<int>(numeric_limits<char>::min());

   public:
    Table() {
        root = new Trie();
        // generate all possible ascii chars - initial alphabet used for encoding & decoding
        for (int i = 0; i < NUM_OF_CHARS; ++i) {
            root->data[char(i)] = new Trie(size++);
        }
    }

    /**
     * Searches the word in the table. If the word exists in the table,
     * returns its value. Otherwise inserts the word and returns the value of the
     * existing prefix of the word and the length of the prefix that exists
     **/
    pair<int, int> get_prefix_and_insert(char* file_data, int start, int end) {
        Trie* t_root = root;
        pair<int, int> ans;
        int common_prefix_length = 0;  // the length of the existing prefix of the word stored in the trie
        for (int i = start; i < end; ++i) {
            char c = file_data[i];
            if (t_root->data[c]) {
                t_root = t_root->data[c];
                ++common_prefix_length;
            } else {
                // no such char, return current node value and insert word in a trie
                t_root->data[c] = new Trie(size++);
                break;
            }
        }
        return {t_root->v, common_prefix_length};
    }
};

char bits_to_char(vector<char>& bits, int start) {
    char r = 0;
    int to = min((int)bits.size(), start + 8);
    for (int i = start; i < to; ++i) {
        r += (bits[i] == '1' ? powers_of_two[i - start] : 0);
    }
    return r;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./lzw <e/d> <file to encode/decode>" << endl;
        return 0;
    }
    string mode = argv[1];
    string filename = argv[2];
    // read file which needs to be processed
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "Error opening file" << endl;
        return 0;
    }
    fin.tie(nullptr);
    // get length of file:
    fin.seekg(0, fin.end);
    int file_length = fin.tellg();
    fin.seekg(0, fin.beg);
    char* file_data = new char[file_length];
    // read file
    fin.read(file_data, file_length);

    if (mode == "e") {
        // encode data
        vector<char> bits;
        // create table (prefix tree)
        Table table;
        int front = 0;
        while (front < file_length) {
            pair<int, int> prefix = table.get_prefix_and_insert(file_data, front, file_length);
            // convert numbers to bits
            int n = prefix.first;
            // convert number to bits
            vector<char> n_bits;
            while (n) {
                n_bits.push_back('0' + (n % 2));
                n /= 2;
            }
            // align bits if the number of bits is less then 7
            for (int i = 0; i < 7 - static_cast<int>(n_bits.size()); ++i) {
                bits.push_back('0');
            }
            // copy n_bits to bits
            bits.insert(bits.end(), n_bits.begin(), n_bits.end());
            // advance front pointer
            front += prefix.second + 1;
        }
        ofstream out("enc.txt");
        // TODO research here, maybe need to call tie to improve performance
        // write bits to file
        for (int i = 0; i < bits.size(); i += 8) {
            // convert 8 bits to one char
            out << bits_to_char(bits, i);
        }

    } else if (mode == "d") {
        // decode data
    } else {
        cout << "Mode must be either \"e\" or \"d\"" << endl;
        return 0;
    }
    return 0;
}
