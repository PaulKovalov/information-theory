#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

int NUM_OF_CHARS = 128;  // consider all ASCII characters

/**
 * Separate table class to encapsulate implementation.
 **/
class Table {
    struct Trie {
        // better to use unique_ptr, but performance may degrade
        // TODO research this
        unordered_map<int, Trie*> data;
        int v;
        Trie(int v = -1) : v(v){};
    };
    Trie* root;
    int size = 0;

   public:
    /**
    * Constructs initial table. Depending on mode, maps chars to numbers or vice versa
    * */
    Table() {
        root = new Trie();
        // generate all possible ascii chars - initial alphabet used for encoding & decoding
        for (int i = 0; i < NUM_OF_CHARS; ++i) {
            root->data[i] = new Trie(size++);
        }
    }

    /**
     * Searches the word in the table. If the word exists in the table,
     * returns its value. Otherwise inserts the word and returns the value of the
     * existing prefix of the word and the length of the prefix that exists
     **/
    pair<int, int> get_prefix_and_insert(char* data, int start, int end) {
        Trie* t_root = root;
        int common_prefix_length = 0;  // the length of the existing prefix of the word stored in the trie
        for (int i = start; i < end; ++i) {
            char c = data[i];
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

    int get_size() {
        return size;
    }
};

int bits_to_int(vector<char>& bits, int start, int bits_per_int) {
    int r = 0;
    int to = min((int)bits.size(), start + bits_per_int);
    int current_power_of_two = 1;
    for (int i = start; i < to; ++i) {
        r += (bits[i] == '1' ? current_power_of_two : 0);
        current_power_of_two *= 2;
    }
    return r;
}

vector<char> char_to_bits(char& c) {
    int r = c;
    if (r < 0) {
        r += 256;
    }
    // char encodes 8 bits
    vector<char> bits(8);
    for (int i = 0; i < 8; ++i) {
        bits[i] = ('0' + (r % 2));
        r /= 2;
    }
    return bits;
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
    fin.close();

    if (mode == "e") {
        // encode data
        vector<char> bits;
        // create table (prefix tree)
        Table table;
        int front = 0, bits_per_number = 8, current_dict_capacity = 256;
        while (front + 1 < file_length) {
            pair<int, int> prefix = table.get_prefix_and_insert(file_data, front, file_length);
            if (table.get_size() == current_dict_capacity) {
                bits_per_number++;
                current_dict_capacity *= 2;
            }
            // convert numbers to bits
            int n = prefix.first;
            // convert number to bits
            vector<char> n_bits;
            while (n) {
                n_bits.push_back('0' + (n % 2));
                n /= 2;
            }
            // copy n_bits to bits
            bits.insert(bits.end(), n_bits.begin(), n_bits.end());
            // align bits if their number is less then bits_per_number
            bits.insert(bits.end(), max(0, bits_per_number - static_cast<int>(n_bits.size())), '0');
            // advance front pointer
            front += prefix.second;
        }
        ofstream out("enc.txt");
        // TODO research here, maybe need to call tie to improve performance
        int bits_per_char = 8;
        // write bits to file
        for (int i = 0; i < bits.size(); i += bits_per_char) {
            // convert 8 bits to one char
            out << static_cast<char>(bits_to_int(bits, i, bits_per_char));
        }
        out.close();
    } else if (mode == "d") {
        vector<char> bits;
        // convert chars in file to a bit array
        for (int i = 0; i < file_length; ++i) {
            vector<char> c_bits = char_to_bits(file_data[i]);
            bits.insert(bits.end(), c_bits.begin(), c_bits.end());
        }
        // convert bits to vector of ints
        int bits_per_number = 8, start = 0, current_max_number = 256;
        vector<int> codes;
        while (start < bits.size()) {
            // read bits_per_number_bits from the stream and convert them to the number
            if (codes.size() + NUM_OF_CHARS == current_max_number - 1) {
                // increase bit length if table is full
                bits_per_number++;
                current_max_number *= 2;
            }
            codes.push_back(bits_to_int(bits, start, bits_per_number));
            start += bits_per_number;
        }
        // generate initial table
        unordered_map<int, string> table;
        for (int i = 0; i < NUM_OF_CHARS; ++i) {
            table[i] = string(1, static_cast<char>(i));
        }
        string decoded = "";
        int code = codes[0], old_code;
        decoded += table[code];
        old_code = code;
        for (int i = 1; i < codes.size(); ++i) {
            code = codes[i];
            // code is in the initial dictionary
            if (table.find(code) != table.end()) {
                decoded += table[code];
                string new_table_entry = table[old_code] + string(1, table[code][0]);
                table[table.size()] = new_table_entry;
            } else {
                table[code] = table[old_code] + string(1, table[old_code][0]);
                decoded += table[code];
            }
            old_code = code;
        }
        // open file dec.txt and write result there
        ofstream out("dec.txt");
        out << decoded;
        out.close();
    } else {
        cout << "Mode must be either \"e\" or \"d\"" << endl;
        return 0;
    }
    return 0;
}
