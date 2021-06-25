//
// Created by mariana on 23/6/21.
//

#ifndef AED2_P3_TECFS_HUFFMAN_H
#define AED2_P3_TECFS_HUFFMAN_H


#include <iostream>
#include <vector>
#include "List.h"

using namespace std;

class LeafNode{
public:
    int frequency;
    char letter;
    string code;
    LeafNode *left, *right;
    LeafNode(int frequency,char letter,LeafNode *left, LeafNode *right);
    LeafNode();
};

class DecodeTreeNode{
public:
    string code;
    char letter;
    DecodeTreeNode *next;
};

//! Class in charge of coding and decoding messages trough Huffman Algorithm
class Huffman
{
public:
    Huffman(string text);
    void decode(LeafNode* root, string str);
    void decodeTemp(LeafNode* root, int &index, string str);


private:
    void huffmanTree(string text);
    void encode(LeafNode* root, string str,DecodeTreeNode*& decode);

};


#endif //AED2_P3_TECFS_HUFFMAN_H
