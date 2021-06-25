//
// Created by mariana on 23/6/21.
//

#ifndef AED2_P3_TECFS_HUFFMAN_H
#define AED2_P3_TECFS_HUFFMAN_H

#include <iostream>
#include <vector>
#include "List.h"

using namespace std;

class LeafNode
{
public:
    int frequency;
    char letter;
    string code;
    LeafNode *left, *right;
    LeafNode(int frequency, char letter, LeafNode *left, LeafNode *right);
    LeafNode() : left(nullptr), right(nullptr) {}
    ~LeafNode()
    {
        if (right != nullptr)
        {
            delete right;
        }
        if (left != nullptr)
        {
            delete left;
        }
    }
    LeafNode &operator=(LeafNode const &other)
    {
        this->frequency = other.frequency;
        this->letter = other.letter;
        // this->code = other.code; no sé si esto se usa
        if (other.right == nullptr)
        {
            this->right = nullptr;
        }
        else
        {
            LeafNode *newRight = new LeafNode();
            *newRight = *(other.right);
            this->left = newRight;
        }
        if (other.left == nullptr)
        {
            this->left = nullptr;
        }
        else
        {
            LeafNode *newLeft = new LeafNode();
            *newLeft = *(other.left);
            this->left = newLeft;
        }
        return *this;
    }
    LeafNode(LeafNode const &other) : left(nullptr), right(nullptr)
    {
        this->frequency = other.frequency;
        this->letter = other.letter;
        if (other.right != nullptr)
        {
            this->right = new LeafNode();
            *(this->right) = *other.right;
        }
        if (other.left != nullptr)
        {
            this->left = new LeafNode();
            *(this->left) = *other.left;
        }
    }
};

// json conversion
void to_json(json &j, const LeafNode &n);
void from_json(const json &j, LeafNode &n);

class DecodeTreeNode
{
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
    static void decode(LeafNode *root, string str, string *decoded);
    LeafNode *getDecodeTree();
    std::string getEncodedMsg();

private:
    static void decodeTemp(LeafNode *root, int &index, string str, string *decoded);
    void huffmanTree(string text);
    void encode(LeafNode *root, string str, DecodeTreeNode *&decode);
    LeafNode *decodificationTree;
    std::string encodedMsg;
};

#endif //AED2_P3_TECFS_HUFFMAN_H
