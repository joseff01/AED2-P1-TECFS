#include "ceROBOT.h"

/**
 * @brief ceROBOT::ceROBOT Constructor of the ceROBOT object. Calls all needed methods to make the object work
 */
ceROBOT::ceROBOT() {
    clientSetup();
    sendFileController();
}

/**
 * @brief ceROBOT::clientSetup Method in charge of setting up connection with ControllerNode app
 */
void ceROBOT::clientSetup() {
    int option = 1;
    struct sockaddr_in serv_addr;
    char const *localHost = "localhost";
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }
    server = gethostbyname(localHost);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host");
        exit(0);
    }
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portNo);
    cout << "Connecting to ControllerNode..." << endl;
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
    }
    string testMessage = receiveMsg();
    cout << testMessage << endl;
    sendMsg("Client Message Received. Connection established!");
}

/**
 * @brief ceROBOT::receiveMsg Wait for a message to arrive from ControllerNode. When it arrives, it returns it
 * @return stringBuffer string of the message received from ControllerNode
 */
string ceROBOT::receiveMsg(){
    memset(buffer, 0, 1025);
    int n = read(sockfd, buffer, 1025);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    string stringBuffer(buffer);
    return stringBuffer;
}

/**
 * @brief ceROBOT::receiveJson Wait for a message to arrive from ControllerNode. When it arrives, it parses it as a json object and returns it
 * @return jsonBuffer json object of the message received from ControllerNode
 */
json ceROBOT::receiveJson(){
    memset(buffer, 0, 1025);
    int n = read(sockfd, buffer, 1025);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    json jsonBuffer = json::parse(buffer);
    return jsonBuffer;
}

/**
 * @brief ceROBOT::sendMsg Sends a string message to ControllerNode
 * @param stringMsg Message to send to ControllerNode
 */
void ceROBOT::sendMsg(string stringMsg) {
    memset(buffer, 0, 1025);
    strncpy(buffer, stringMsg.c_str(), 1025);
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}

/**
 * @brief ceROBOT::sendFileController main ceROBOT method that, given a directory trough the console, will search all files in the
 * directory and will send the name of every file and it's contents to ControllerNode
 */
void ceROBOT::sendFileController() {
    while(true){
        cout << "Enter directory of files to add to the RAID system: " << endl;
        cin >> libPath;
        if (libPath == "EXIT"){
            break;
        }
        for (const auto & entry : fs::directory_iterator(libPath)) {
            cout << entry.path() << endl;
            string fileName = entry.path().filename();
            cout << fileName << endl;
            ifstream file(entry.path());
            stringstream streamBuffer;
            streamBuffer << file.rdbuf();
            string fileContents = streamBuffer.str();
            cout << fileContents;
            json fileJson;
            fileJson["case"] = CEROBOT_REQUEST;
            /*
             *
             *
             * HUFFMAN ENCODING OF FILENAME AND CONTENTS
             *
             *
             */
            //fileJson["fileName"] = encodedFileName;
            //fileJson["fileContents"] = encodedFileContents;
            /*
             *
             * SEND MESSAGE TO CONTROLLER NODE
             *
             *
             */
        }
    }
}
