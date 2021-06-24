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

using namespace std;
using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

class ceROBOT {

private:
    int portNo = 5000;
    int sockfd;
    char buffer[1025];
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
