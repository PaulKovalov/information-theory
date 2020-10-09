#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

using namespace std;
int NUM_OF_CHARS = 256;

/**
 * Separate table class to encapsulate implementation. If I want to use Trie for storing table instead of plain
 * unordered map, will just change implementation here
 **/
class Table {
    unordered_map<string, int> table;
    // initializes the table
   public:
    Table(char* file_data, int file_length) {
        // prepare chars array
        char* file_chars = new char[NUM_OF_CHARS];
        memset(file_chars, 0, NUM_OF_CHARS);
        for (int i = 0; i < file_length; ++i) {
            if (file_chars[file_data[i] + 128] == 0) {
                file_chars[file_data[i] + 128] = 1;
                table[string(1, file_data[i])] = table.size();
            }
        }
    }

    /**
     * Searches the word in the table. If the word exists in the table,
     * returns its value. Otherwise inserts the word and returns the value of the
     * existing prefix of the word and the length of the prefix that exists
     **/
    pair<int,int> get_or_insert(string word) {
        string word_copy = word;
        while (word_copy.size() && table.find(word_copy) == table.end()) {
            word_copy.pop_back();
        }
        // if word is not in the table, it means its size was reduced. Insert word in the table then, but return the 
        // value of the found prefix
        if (word_copy.size() != word.size()) {
            table[word] = table.size();
        }
        // first is value in the table, second is the length of the matched prefix
        return {table[word_copy], word_copy.size()};
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./lzw <e/d> <file to encode/decode>" << endl;
        return 0;
    }
    string mode = argv[1];
    string filename = argv[2];
    // read file which needs to be processed
    ifstream fin(filename);
    fin.tie(nullptr);
    if (!fin.is_open()) {
        cout << "Error opening file" << endl;
        return 0;
    }
    // get length of file:
    fin.seekg(0, fin.end);
    int file_length = fin.tellg();
    fin.seekg(0, fin.beg);
    char* file_data = new char[file_length];
    // read file
    fin.read(file_data, file_length);
    
    if (mode == "e") {
        // encode data
        Table t(file_data, file_length);

    } else if (mode == "d") {
        // decode data
    } else {
        cout << "Mode must be either \"e\" or \"d\"" << endl;
        return 0;
    }
    return 0;
}
