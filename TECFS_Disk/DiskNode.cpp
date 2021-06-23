#include "DiskNode.h"

DiskNode::DiskNode(int diskNum) {
    //Read XML for Info
    XMLDocument XMLDoc;
    switch (diskNum) {
        case 1: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk1.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
        case 2: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk2.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
        case 3: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk3.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
        case 4: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk4.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
        case 5: {
            XMLError err = XMLDoc.LoadFile("../../SetupXML/Disk5.xml");
            if (err != XML_SUCCESS) {
                cout << "Error loading file: " << (int) err << endl;
                cout << "Error loading file: " << XMLDoc.ErrorName() << endl;
            }
            break;
        }
    }
    XMLNode *pRoot = XMLDoc.FirstChild();
    XMLElement *pSetupElements = pRoot->FirstChildElement("SetupElements");
    XMLElement *pElement = pSetupElements->FirstChildElement();
    pElement->QueryIntText(&portNo);
    cout << "portNo: "<< portNo << endl;
    pElement = pElement->NextSiblingElement();
    libPath = pElement->GetText();
    cout << "Path: "<<libPath << endl;

    //Setup client-server connection
    clientSetup();
    receiveMsg();
}

void DiskNode::clientSetup() {
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

string DiskNode::receiveMsg(){
    memset(buffer, 0, 1025);
    int n = read(sockfd, buffer, 1025);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    string stringBuffer(buffer);
    return stringBuffer;
}

void DiskNode::sendMsg(string stringMsg) {
    memset(buffer, 0, 1025);
    strncpy(buffer, stringMsg.c_str(), 1025);
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}
