//
// Created by mariana on 23/6/21.
//
#include "UtilityText.h"
#include "Huffman.h"

Huffman::Huffman(string text){
    huffmanTree(text);
};

void print_list(DecodeTreeNode* n)
{
    cout << "\nPrinting new list..." << endl;
    while (n != nullptr) {
        cout << n->letter << " " << n->code <<endl;
        n = n->next;
    }
}

/**
 * \brief Method in charge of coding a recieved message
 * @param og Original string recieved
 * @param root linked list with characters and encodded string that represents each character
 * @return encoded original string
 */
string codedMessage(string og, DecodeTreeNode* root)
{
    string str = "";
    cout << "Message coded:" << endl;
    for(int i = 0; i<og.size();i++){
        DecodeTreeNode* temp = root;
        char ch = og[i];
        while(root != nullptr)
        {
            if(temp->letter==ch)
            {
                str+= temp->code;
                break;
            }
            temp = temp->next;
        }
    }
    cout<< str<< endl;
    return str;

}
/**
 * \brief auxiliar method that call decodeTemp. This method prints the decoded string
 * @param root Binary Tree to decode a recieved string
 * @param str encoded string given by the Hoffman Algorithm
 */
void Huffman::decode(LeafNode* root, string str) {
    int index = -1;
    cout << "\nDecoded string is: \n";
    while (index < (int) str.size() - 1) {
        decodeTemp(&root[0], index, str);
    }
}

/**
 * \brief Method that prints decoded string
 * @param root Binary Tree to decode recieved string
 * @param index key that indicates iterative index for string
 * @param str encoded string given by the Hoffman Algorithm
 */
void Huffman::decodeTemp(LeafNode* root, int &index, string str)
{
    if (root == nullptr) { //end of recursion
        return;
    }

    // found a leaf node
    if (!root->left && !root->right){
        cout << root->letter;
        return;
    }
    index++;

    if (str[index] =='0')
        decodeTemp(root->left, index, str);
    else
        decodeTemp(root->right, index, str);
}

/**
 * \brief method that creates a linked list with both char, and code for its respective char
 * @param root Binary Tree of the original string
 * @param str recieved string
 * @param decode decoding linked list
 */
void Huffman::encode(LeafNode* root, string str,DecodeTreeNode*& decode)
{
    if (root == nullptr)
        return;

    //found a leaf node
    if (root->left == nullptr && root->right == nullptr)
    {
        if(decode->letter==NULL){ //first node of linked list
            decode->code= str;
            decode->letter = root->letter;
        }
        else{                     //New nodes
            DecodeTreeNode* temp = new DecodeTreeNode();
            temp->letter = root->letter;
            temp->code = str;
            temp->next = decode;
            decode = temp;
        }
    }
    encode(root->left, str + "0", decode);
    encode(root->right, str + "1", decode);
}

/**
 * \brief HuffmanTree's constructor to encode a recieved message
 * @param text string recieved
 */
void Huffman::huffmanTree(string text) {
    string og = text;
    UtilityText *a = new UtilityText();
    ProcessNode *arrangedList = a->processText(text); //

    // Change ProcessNode to LeafNode
    List<LeafNode *> root;                  // Array with nodes
    while (arrangedList != nullptr) {
        LeafNode *temp = new LeafNode(arrangedList->frequency, arrangedList->letter, nullptr, nullptr);
        root.push_back(temp);
        arrangedList = arrangedList->next;
    }

    //Build Tree
    while (root.length() != 1) {            // Final length = 1
        LeafNode *left = root[0];
        root.erase(0);
        LeafNode *right = root[0];
        root.erase(0);
        int freq = left->frequency + right->frequency;

        LeafNode *temp = new LeafNode(freq, '\0', left, right);
        root.push_back(temp);

    }

    auto *decoded = new DecodeTreeNode();
    encode(root[0], "", decoded);   // Encodes original string

    //print_list(decoded);

    string str = codedMessage(og, decoded); // Final coded string
    //decode(root[0],str);
}



LeafNode::LeafNode(int frequency, char letter, LeafNode *left, LeafNode *right)
{
    this->frequency = frequency;
    this->letter = letter;
    this->left = left;
    this->right = right;
}