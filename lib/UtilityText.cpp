//
// Created by mariana on 23/6/21.
//
#include "UtilityText.h"
void print_list(ProcessNode* n)
{
    cout << "\nPrinting new list..." << endl;
    while (n != nullptr) {
        cout << n->letter << " " << n-> frequency <<endl;
        n = n->next;
    }
}

/**
 * \brief changes position of nodes
 * @param a left node
 * @param b right node
 */
void swap(ProcessNode* a, ProcessNode* b)
{
    ProcessNode t = *a;
    *a = *b;
    *b = t;
}

/**
 * \brief bubbleSort algorithm arranges according to the frequency and placement of the algorithm
 * @param arr array with nodes
 * @param n size of array
 */
void bubbleSort(ProcessNode* arr[], int n)
{
    int i, j;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j]->frequency > arr[j + 1]->frequency)
                swap(arr[j], arr[j + 1]);
        }
    }
}


void printA(ProcessNode* arr[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        cout << arr[i]->frequency << " : " << arr[i]->letter;
    cout << endl;
}

/**
 * \brief links nodes of array in a linkedlist
 * @param arr array of arranged nodes
 * @param len size of array
 * @return linkedlist with arranged nodes
 */
ProcessNode* linkNodes(ProcessNode* arr[], int len)
{
    ProcessNode* current;

    for(int i=0; i<len; i++)
    {
        current = arr[i];
        if(i == len-1)
        {
            current->next = nullptr;
        }
        else
            current->next = arr[i+1];
    }
    return arr[0];


}

/**
 * \brief separates nodes and adds it to an array
 * @param n linked list with disordered elements
 * @param len size of array
 * @return array with nodes
 */
ProcessNode* prepList(ProcessNode *n,int len){

int counter = 0;

ProcessNode* arr[len];

while(n!= nullptr)
{
    arr[counter] = n;
    n= n->next;
    counter++;
}

bubbleSort(arr,len);
//printA(arr,len);

return linkNodes(arr,len);

}

/**
 * \brief calculates frequency of each character in a string and calls methods and fucntions to
 * arrange the list according to their frequency
 * @param text recieved string
 * @return arranged linkedlist
 */
ProcessNode* UtilityText::processText(string text){
    ProcessNode* start = new ProcessNode();

    start -> frequency = 1;
    start -> letter = text[0];
    int len = text.size();
    for(int i =1; i<len;i++) {
        bool notFound = true;
        char currentChar = text[i];
        ProcessNode* temp = start;

        while (temp->next != NULL) {
            if (temp->letter == currentChar){ break;}
            else { temp = temp->next; }
        }
        if(temp->letter == currentChar)
        {
            temp->frequency++;
            notFound = false;
        }
        if (notFound) {
            ProcessNode *newNode = new ProcessNode();
            newNode->letter = currentChar;
            newNode->frequency = 1;
            temp->next = newNode;
        }
    }

    //print_list(start);

    ProcessNode* n = start;

    int counter = 0;
    while (n != nullptr) {
        counter++;
        n = n->next;
    }
    ProcessNode* arranged =prepList(start,counter);
    return arranged;
}

