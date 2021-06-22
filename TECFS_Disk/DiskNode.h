#ifndef TECFS_DISK_DISKNODE_H
#define TECFS_DISK_DISKNODE_H

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "../lib/tinyxml2.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

using namespace std;
using namespace tinyxml2;

class DiskNode {

private:
    int portNo;
    int sockfd;
    char buffer[1025];
    string libPath;
    void clientSetup();
    string receiveMsg();
    void sendMsg(string message);
public:
    explicit DiskNode(int diskNum);

};


#endif //TECFS_DISK_DISKNODE_H
