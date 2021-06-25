#include "ceROBOT.h"
#include "../lib/Huffman.h"

/**
 * @brief ceROBOT::ceROBOT Constructor of the ceROBOT object. Calls all needed methods to make the object work
 */
ceROBOT::ceROBOT()
{
    clientSetup();
    sendFileController();
}

/**
 * @brief ceROBOT::clientSetup Method in charge of setting up connection with ControllerNode app
 */
void ceROBOT::clientSetup()
{
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
string ceROBOT::receiveMsg()
{
    memset(buffer, 0, BUFFER_SIZE);
    int n = read(sockfd, buffer, BUFFER_SIZE);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    std::string receivedMsg;
    if (n == 0)
    {
        std::cout << "Received empty string\n";
        receivedMsg = json({{"Case", CLOSE}}).dump();
    }
    else
    {
        std::string encodedMsg = std::string(buffer);
        memset(buffer, 0, BUFFER_SIZE);
        int n = read(sockfd, buffer, BUFFER_SIZE);
        if (n == 0)
        {
            std::cout << "Received empty string\n";
            receivedMsg = json({{"Case", CLOSE}}).dump();
            return receivedMsg;
        }

        json treeJSON = json::parse(buffer);
        string x = "";
        string *deco = &x;
        LeafNode tree = treeJSON.get<LeafNode>();
        Huffman::decode(&tree, encodedMsg, deco);
        receivedMsg = *deco;
    }
    std::cout << "Message received: " << receivedMsg << std::endl;
    return receivedMsg;
}

/**
 * @brief ceROBOT::receiveJson Wait for a message to arrive from ControllerNode. When it arrives, it parses it as a json object and returns it
 * @return jsonBuffer json object of the message received from ControllerNode
 */
json ceROBOT::receiveJson()
{
    memset(buffer, 0, BUFFER_SIZE);
    int n = read(sockfd, buffer, BUFFER_SIZE);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    json jsonBuffer;
    if (n == 0)
    {
        std::cout << "Received empty string\n";
        jsonBuffer = json({{"Case", CLOSE}});
    }
    else
    {
        std::string encodedMsg = std::string(buffer);
        memset(buffer, 0, BUFFER_SIZE);
        int n = read(sockfd, buffer, BUFFER_SIZE);
        if (n == 0)
        {
            std::cout << "Received empty string\n";
            jsonBuffer = json({{"Case", CLOSE}});
            return jsonBuffer;
        }

        json treeJSON = json::parse(buffer);
        string x = "";
        string *deco = &x;
        LeafNode tree = treeJSON.get<LeafNode>();
        Huffman::decode(&tree, encodedMsg, deco);
        jsonBuffer = json::parse(*deco);
    }
    cout << "Received JSON: " << jsonBuffer.dump() << endl;
    return jsonBuffer;
}

/**
 * @brief ceROBOT::sendMsg Sends a string message to ControllerNode
 * @param stringMsg Message to send to ControllerNode
 */
void ceROBOT::sendMsg(string stringMsg)
{
    Huffman huff(stringMsg);
    LeafNode *root = huff.getDecodeTree();
    std::string encodedMsg = huff.getEncodedMsg();
    json j = *root;
    std::string treeMsg = j.dump();

    sleep(0.4);
    if (send(sockfd, encodedMsg.c_str(), strlen(encodedMsg.c_str()), 0) != strlen(encodedMsg.c_str()))
    {
        perror("send");
    }
    sleep(0.4);
    if (send(sockfd, treeMsg.c_str(), strlen(treeMsg.c_str()), 0) != strlen(treeMsg.c_str()))
    {
        perror("send");
    }
}

/**
 * @brief ceROBOT::sendFileController main ceROBOT method that, given a directory trough the console, will search all files in the
 * directory and will send the name of every file and it's contents to ControllerNode
 */
void ceROBOT::sendFileController()
{
    while (true)
    {
        cout << "Enter directory of files to add to the RAID system: " << endl;
        cin >> libPath;
        if (libPath == "EXIT")
        {
            break;
        }
        for (const auto &entry : fs::directory_iterator(libPath))
        {
            cout << entry.path() << endl;
            string fileName = entry.path().filename();
            cout << fileName << endl;
            ifstream file(entry.path());
            stringstream streamBuffer;
            streamBuffer << file.rdbuf();
            string fileContents = streamBuffer.str();
            cout << fileContents;
            json fileJson;
            fileJson["Case"] = CEROBOT_REQUEST;
            fileJson["FileName"] = fileName;
            fileJson["FileContents"] = fileContents;
            string jsonSend = fileJson.dump();
            sendMsg(jsonSend);
            receiveMsg();
        }
    }
}
