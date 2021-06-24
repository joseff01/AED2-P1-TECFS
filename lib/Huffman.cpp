//
// Created by mariana on 23/6/21.
//
#include "UtilityText.h"
#include "Huffman.h"

Huffman::Huffman(string text){
    huffmanTree(text);
};

void print_list(List<LeafNode*> n)
{
    cout << "\nPrinting new list..." << endl;
    for(int i = 0; i<n.length();i++)
    {
        cout << n[i]->letter << " " << n[i]->frequency <<endl;
    }
}
void print_list(List<string*> n)
{
    cout << "Printing new list..." << endl;
    for(int i = 0; i<n.length();i++)
    {
        cout << n[i] <<endl;
    }
}

void print_list(List<string> n, List<char> m)
{
    cout << "list of strings" << endl;
    for(int i = 0; i<n.length();i++)
    {
        cout << m[i]<< " : "<< n[i] <<endl;
    }
}
void print_list(DecodeTreeNode* n)
{
    cout << "\nPrinting new list..." << endl;
    while (n != nullptr) {
        cout << n->letter << " " << n->code <<endl;
        n = n->next;
    }
}

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
void decode(LeafNode* root, int &index, string str)
{
    if (root == nullptr) {
        return;
    }

    // found a leaf node
    if (!root->left && !root->right)
    {
        cout << root->letter;
        return;
    }

    index++;

    if (str[index] =='0')
        decode(root->left, index, str);
    else
        decode(root->right, index, str);
}



void Huffman::encode(LeafNode* root, string str,DecodeTreeNode*& decode)
{
    if (root == nullptr)
        return;

/*
    cout << root->letter << "  " << root->frequency << endl;
    cout << root->left<< "  " << root->right << endl;
    cout << "done" << endl;
*/

    //found a leaf node
    if (root->left == nullptr && root->right == nullptr)
    {
        if(decode->letter==NULL)
        {
            decode->code= str;
            decode->letter = root->letter;
        }
        else
        {
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


void Huffman::huffmanTree(string text)
{
    string og = text;
    UtilityText* a = new UtilityText();
    ProcessNode* arrangedList = a->processText(text);

    ProcessNode* arrangedTemp = arrangedList;
    List<LeafNode*> root;
    while(arrangedList != nullptr)
    {
        LeafNode* temp = new LeafNode(arrangedList ->frequency, arrangedList ->letter, nullptr, nullptr);
        root.push_back(temp);
        arrangedList = arrangedList ->next;
    }

    //Build Tree
    while(root.length() != 1)
    {
        LeafNode* left = root[0];
        root.erase(0);
        LeafNode *right = root[0];
        root.erase(0);
        int freq = left->frequency + right->frequency;

        LeafNode* temp = new LeafNode(freq,'\0',left,right);
        root.push_back(temp);

    }

    auto* decoded = new DecodeTreeNode();
    encode(root[0],"", decoded);
    print_list(decoded);
    string str = codedMessage(og,decoded);

    int index = -1;
    cout << "\nDecoded string is: \n";

    while (index < (int)str.size()-1) {
        decode(root[0],index,str);
    }
}

LeafNode::LeafNode(int frequency, char letter, LeafNode *left, LeafNode *right)
{
    this->frequency = frequency;
    this->letter = letter;
    this->left = left;
    this->right = right;
}