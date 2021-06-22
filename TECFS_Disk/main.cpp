#include <iostream>
#include "DiskNode.h"
#include "../lib/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

int main() {

    cout << "Input Disk To Manage:" << endl;
    int diskNum;
    bool validInputFlag = true;
    while (validInputFlag){
        cin >> diskNum;
        if ((1 <= diskNum) && (diskNum <= 4)){
            cout << "Starting Disk " << diskNum << " setup..." << endl;
            validInputFlag = false;
        } else{
            cout << "Disk " << diskNum << " not found. Musk be Disk 1 - 4" << endl;
            continue;
        }
    }
    DiskNode diskNode(diskNum);

}
