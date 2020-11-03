Compile program with
g++ huffman.cpp -o huffman -std=c++11

To encode a file, run a program with
./huffman e <num of symbols per word> <input file>

After encoding, program generates a file "enc.txt" which contains encoded text

To decoded a file, run a program with
./huffman d <num of symbols per word> enc.txt

After decoding, program generates a file "dec.txt" which contains decoded text
