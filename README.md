This repository provides a few algorithms for encoding information

## Huffman coding

Compile program with
`g++ huffman.cpp -o huffman -std=c++11 -O3`

To encode a file, run a program with
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

Program generates a file `enc.txt` which contains encoded text.
To decoded a file, run a program with
`./huffman d <num of symbols per word> enc.txt`

Decoding produces file "dec.txt" which contains decoded text.


## LZW

Compile program with
`g++ lzw.cpp -o lzw -std=c++11 -O3`

#todo

## RSA

#todo

## Hamming code

#todo
