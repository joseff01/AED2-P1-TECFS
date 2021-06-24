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

class UtilityText {
public:
    ProcessNode* processText(string text);
    void bubbleSort(LeafNode* arr[], int n);
private:
    void swap(LeafNode* a, LeafNode* b);
    //LeafNode*arrangeList(LeafNode *node);

};



#endif //AED2_P3_TECFS_UTILITYTEXT_H
