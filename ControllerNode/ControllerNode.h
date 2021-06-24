#ifndef CONTROLLERNODE_CONTROLLERNODE_H
#define CONTROLLERNODE_CONTROLLERNODE_H

#include <iostream>
#include "../lib/tinyxml2.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <bitset>

#include "../lib/RequestConstants.h"
#include "../lib/List.h"

class ControllerNode
{
private:
    int portNo = 5000;
    int sockfd, newsockfd, maxsd, socketDescriptor, activity, valread;
    // 0-4 disknodes, 5 ceSearch, 6 ceRobot
    int clientSocket[7];
    fd_set readfds;
    char buffer[1025];
    void serverSetup();
    std::string receiveMsg(int receiveSockfd);
    void sendMsg(int sendSockfd, std::string Msg);

    //binary conversion
    std::string bin2letters(std::string str);
    std::string letters2bin(std::string str);

    // store file
    void storeFile(std::string fileName, std::string fileContents);

public:
    ControllerNode();
};

#endif //CONTROLLERNODE_CONTROLLERNODE_H
