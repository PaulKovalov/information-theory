#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./hamming <e/d> <bits to encode/decode>" << endl;
        return 0;
    }
    string mode(argv[1]);
    string bits(argv[2]);
    if (mode == "e") {
        int message_length = static_cast<int>(bits.size());
        int k = floor(log(message_length) / log(2)) + 1;
        message_length += k;
        int gap = 1;
        vector<vector<int>> parity_codes_positions(k);
        for (int i = 0; i < k; ++i) {
            for (int j = 0; (j + gap - 1) < message_length; ++j) {
                if ((j / gap) % 2 == 0) {
                    parity_codes_positions[i].push_back(j + gap - 1);
                }
            }
            gap *= 2;
        }
        // insert 0s on places where control bits will be inserted
        string bits_encoded = "";
        auto bits_iter = bits.begin();
        int power_of_two = 1, i = 1;
        while (bits_iter != bits.end()) {
            if (i == power_of_two) {
                bits_encoded += "0";
                power_of_two *= 2;
            } else {
                bits_encoded += *bits_iter;
                bits_iter = next(bits_iter);
            }
            ++i;
        }
        vector<int> parity_codes_values;
        for (int i = 0; i < parity_codes_positions.size(); ++i) {
            int _xor = 0;
            for (int j = 0; j < parity_codes_positions[i].size(); ++j) {
                int bit_position = parity_codes_positions[i][j];
                _xor += (bits_encoded[bit_position] - '0');
            }
            parity_codes_values.push_back(_xor % 2);
        }
        for (int i = 0; i < parity_codes_values.size(); ++i) {
            bits_encoded[pow(2, i) - 1] = parity_codes_values[i] + '0';
        }
        cout << bits_encoded << endl;
    } else {
    }
    return 0;
}
