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

using namespace std;

class ControllerNode {
private:
    int portNo = 5000;
    int sockfd, newsockfd ,clientSocket[7], maxsd, socketDescriptor, activity, valread;
    fd_set readfds;
    char buffer[1025];
    void serverSetup();
    string receiveMsg(int receiveSockfd);
    void sendMsg(int sendSockfd, string Msg);
public:
    ControllerNode();
};


#endif //CONTROLLERNODE_CONTROLLERNODE_H
