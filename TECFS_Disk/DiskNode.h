#ifndef TECFS_DISK_DISKNODE_H
#define TECFS_DISK_DISKNODE_H

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>

#include "../lib/tinyxml2.h"
#include "../lib/RequestConstants.h"
#include "../lib/json.hpp"
#include "DataBlock.h"
#include "Metadata.h"

using namespace std;
using namespace tinyxml2;
using json = nlohmann::json;

/**
 * @brief DiskNode Object that manages completely a single disk of the Raid system
 */
class DiskNode {

private:
    /**
     * @brief portNo Number of port to connect to ControllerNode
     */
    int portNo;
    /**
     * @brief sockfd int identifier to communicate with ControllerNode
     */
    int sockfd;
    /**
     * @brief buffer char array that's used to send and receive the data from ControllerNode
     */
    char buffer[1025];
    /**
     * @brief libPath stores the path of the directory to analize
     */
    string libPath;
    void clientSetup();
    string receiveMsg();
    json receiveJson();
    void sendMsg(string message);
    void saveFile(json jsonMessage);
    void recoverFile(json jsonMessage);
    void recoverFileAmount(json jsonMessage);
    void recoverFileMetadata(json jsonMessage);
public:
    explicit DiskNode(int diskNum);

};


#endif //TECFS_DISK_DISKNODE_H
