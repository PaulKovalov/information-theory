This repository provides a few algorithms for encoding information

## Huffman coding

Implementation of the Huffman code for data compression https://en.wikipedia.org/wiki/Huffman_coding
Compile program with
`g++ huffman.cpp -o huffman -std=c++11 -O3`

To encode a file, run the binary with
`./huffman e <number of symbols per word> <input file>`

Number of symbols per word denotes the size of chunk, which will be used as an encoding unit. For example,
for chunk size = 1, each char of text will be encoded, so for the string
`abacab`, the sequence that will be encoded is
```
a
b
a
c
a
b
```
 For chunk size = 2, chars will be taken in pairs, so the sequence that will be encoded is
```
ab 
ac
ab
```
For huge texts, usually bigger values give better compression.

Program generates file `enc.txt` which contains encoded text.
To decoded a file, run a program with
`./huffman d <num of symbols per word> enc.txt`

Decoding produces file `dec.txt` which contains decoded text.

## LZW

Implementation of the Lempel-Ziv-Welch compression algorithm https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch
Compile program with
`g++ lzw.cpp -o lzw -std=c++11 -O3`

To encode a file, run the binary with
`./lzw e <file to encode>`

This will produce file `enc.txt` which contains encoded data. To decode it, run
`./lzw d enc.txt`, and `dec.txt` with decoded text will be generated

## RSA

Doesn't work at the moment

## Hamming code

Implementation of the Hamming code https://en.wikipedia.org/wiki/Hamming_code
Compile program with `g++ hamming.cpp -o hamming -O3 -std=c++11`

To encode a sequence of bits, run
`./hamming e <bits>`

To decode a encoded sequence of bits, run

`./hamming d <bits>`

If the bits sequence has a single error, it will be corrected.
