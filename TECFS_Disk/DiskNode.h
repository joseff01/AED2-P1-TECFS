#ifndef TECFS_DISK_DISKNODE_H
#define TECFS_DISK_DISKNODE_H

#include <iostream>
#include "../lib/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

class DiskNode {

private:
    int portNo;
    string libPath;
public:
    explicit DiskNode(int diskNum);

};


#endif //TECFS_DISK_DISKNODE_H
