#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>> get_parity_codes_positions(int message_length, int k) {
    int gap = 1;
    vector<vector<int>> v(k);
    for (int i = 0; i < k; ++i) {
        for (int j = 0; (j + gap - 1) < message_length; ++j) {
            if ((j / gap) % 2 == 0) {
                v[i].push_back(j + gap - 1);
            }
        }
        gap *= 2;
    }
    return v;
}

vector<int> get_code_values(string& bits, vector<vector<int>> parity_codes_positions) {
    vector<int> parity_codes_values;
    for (int i = 0; i < parity_codes_positions.size(); ++i) {
        int _xor = 0;
        for (int j = 0; j < parity_codes_positions[i].size(); ++j) {
            int bit_position = parity_codes_positions[i][j];
            _xor += (bits[bit_position] - '0');
        }
        parity_codes_values.push_back(_xor % 2);
    }
    return parity_codes_values;
}

string remove_control_bits(string& bits) {
    string result = "";
    int power_of_two = 1;
    for (int i = 0; i < bits.size(); ++i) {
        if (i == power_of_two - 1) {
            power_of_two *= 2;
        } else {
            result += bits[i];
        }
    }
    return result;
}
int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: ./hamming <e/d> <bits to encode/decode>" << endl;
        return 0;
    }
    string mode(argv[1]);
    string bits(argv[2]);
    int message_length = static_cast<int>(bits.size());
    int k = floor(log(message_length) / log(2)) + 1;

    if (mode == "e") {
        message_length += k;
        vector<vector<int>> parity_codes_positions = get_parity_codes_positions(message_length, k);
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
        vector<int> parity_codes_values = get_code_values(bits_encoded, parity_codes_positions);
        for (int i = 0; i < parity_codes_values.size(); ++i) {
            bits_encoded[pow(2, i) - 1] = parity_codes_values[i] + '0';
        }
        cout << bits_encoded << endl;
    } else {
        // calculate the control bits of the received message
        vector<int> actual_parity_codes_values;
        int power_of_two = 1;
        for (int i = 0; i < message_length; ++i) {
            if (i == power_of_two - 1) {
                actual_parity_codes_values.push_back(bits[i] - '0');
                power_of_two *= 2;
                // zeroify control bits
                bits[i] = '0';
            }
        }
        vector<int> correct_parity_codes_values = get_code_values(bits, get_parity_codes_positions(message_length, k));
        int error_value = -1;
        bool has_error = false;
        for (int i = 0; i < correct_parity_codes_values.size(); ++i) {
            int correct = correct_parity_codes_values[i];
            int actual = actual_parity_codes_values[i];
            if (correct != actual) {
                has_error = true;
                cout << "Invalid control bit at position " << pow(2, i) - 1 << endl;
                error_value += pow(2, i);
            }
        }
        if (has_error) {
            cout << "Message had an error at position " << error_value << endl;
            bits[error_value] = (bits[error_value] == '0' ? '1' : '0');
        } else {
            cout << "Message doesn't have errors" << endl;
        }
        cout << remove_control_bits(bits) << endl;
    }
    return 0;
}
