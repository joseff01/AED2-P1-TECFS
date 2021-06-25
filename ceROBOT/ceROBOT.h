#ifndef CEROBOT_CEROBOT_H
#define CEROBOT_CEROBOT_H

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>
#include <string>
#include <experimental/filesystem>

#include "../lib/json.hpp"
#include "../lib/RequestConstants.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

/**
 * @brief ceRobot Interface to insert text files into the RAID filesystem through communication with ControllerNode
 */
class ceROBOT
{

private:
    /**
     * @brief portNo Number of port to connect to ControllerNode
     */
    int portNo = 5000;
    /**
     * @brief sockfd int identifier to communicate with ControllerNode
     */
    int sockfd;
    /**
     * @brief buffer char array that's used to send and receive the data from ControllerNode
     */
    char buffer[BUFFER_SIZE];
    /**
     * @brief libPath stores the path of the directory to analize
     */
    string libPath;
    void clientSetup();
    string receiveMsg();
    json receiveJson();
    void sendMsg(string message);

    void sendFileController();

public:
    ceROBOT();
};

#endif //CEROBOT_CEROBOT_H
