#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<char> divide(vector<char> bits, vector<char>& polynomial) {
    int first_one_idx = find(bits.begin(), bits.end(), 1) - bits.begin();
    while (first_one_idx <= bits.size() - polynomial.size()) {
        for (int i = 0; i < polynomial.size(); ++i) {
            char bit_left = bits[first_one_idx];
            char bit_right = polynomial[i];
            char xored = bit_left ^ bit_right;
            bits[first_one_idx] = xored;
            ++first_one_idx;
        }
        first_one_idx = find(bits.begin(), bits.end(), 1) - bits.begin();
    }
    return bits;
}

int main(int argc, char* argv[]) {
    // crc 5 epc = x^5 + x^3 + 1
    if (argc != 3) {
        cout << "Usage: ./crc-5-epc <mode> <bits>" << endl;
        return 0;
    }
    string mode = string(argv[1]);
    vector<char> bits;
    for (int i = 0; i < strlen(argv[2]); ++i) {
        bits.push_back(argv[2][i] - '0');
    }
    vector<char> polynomial = {1, 0, 1, 0, 1};
    int k_zeros = polynomial.size();  // number of zeros is determined by the generating polynomial

    if (mode == "e") {
        bits.insert(bits.end(), k_zeros, 0);
        vector<char> result = divide(bits, polynomial);
        // now xor the resulted bit sequence with the initial bit sequence
        for (int i = 0; i < bits.size(); ++i) {
            bits[i] = bits[i] ^ result[i];
            cout << static_cast<int>(bits[i]);
        }
        cout << endl;
    } else if (mode == "d") {
        vector<char> result = divide(bits, polynomial);
        vector<char> crc;
        copy_n(result.begin() + (bits.size() - k_zeros), k_zeros, back_inserter(crc));
        cout << "decoded message: ";
        for (int i = 0; i < bits.size() - k_zeros; ++i) {
            cout << static_cast<int>(bits[i]);
        }
        cout<< endl;
        cout << "checksum: ";
        bool has_error = false;
        for (int i = 0; i < crc.size(); ++i) {
            cout << static_cast<int>(crc[i]);
            if (static_cast<int>(crc[i]) == 1) {
                has_error = true;
            }
        }
        cout << endl;
        if (has_error) {
            cout << "Message has ERROR" << endl;
        } else {
            cout << "Message is correct" << endl;
        }
    }
    return 0;
}
