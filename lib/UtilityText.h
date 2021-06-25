//
// Created by mariana on 23/6/21.
//

#ifndef AED2_P3_TECFS_UTILITYTEXT_H
#define AED2_P3_TECFS_UTILITYTEXT_H

#include <iostream>
using namespace std;
#include "Huffman.h"


class ProcessNode
{
public:
    int frequency;
    char letter = NULL;
    ProcessNode  *next = NULL;
};

//! Clas in charge of sroting and arrainging a given string
class UtilityText {
public:
    ProcessNode* processText(string text);
private:
    void swap(LeafNode* a, LeafNode* b);


};



#endif //AED2_P3_TECFS_UTILITYTEXT_H
