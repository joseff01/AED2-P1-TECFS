#include "DiskNode.h"

/**
 * @brief DiskNode::DiskNode Constructor of the DiskNode object. Searches through it's indicated XMLfile with the info needed to connect
 * with ControllerNode. After the main configuration is done, it starts a loop that will always wait for a request from ControllerNode
 * @param diskNum integer that indicates the number of disk that it will manage. (1-5)
 */
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
    //clientSetup();

    //RequestLoop
    /*
    bool closeFlag = true;
    while (closeFlag){
        json jsonMessage = receiveJson();
        switch ((int)jsonMessage["Case"]) {
            case CLOSE:{
                closeFlag = false;
                break;
            }
            case SAVE:{
                saveFile(jsonMessage);
                break;
            }
            case FILE_FROM_NUM:{
                recoverFile(jsonMessage);
                break;
            }
            case METADATA_FROM_NUM:{
                recoverFileMetadata(jsonMessage);
                break;
            }
            case FILE_AMOUNT:{
                recoverFileAmount(jsonMessage);
                break;
            }
        }
    }
     */
    json jsonMessage;
    recoverFile(jsonMessage);
}

/**
 * @brief DiskNode::clientSetup Method in charge of setting up connection with ControllerNode app
 */
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

/**
 * @brief DiskNode::receiveMsg Wait for a message to arrive from ControllerNode. When it arrives, it returns it
 * @return stringBuffer string of the message received from ControllerNode
 */
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

/**
 * @brief DiskNode::receiveJson Wait for a message to arrive from ControllerNode. When it arrives, it parses it as a json object and returns it
 * @return jsonBuffer json object of the message received from ControllerNode
 */
json DiskNode::receiveJson(){
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
 * @brief DiskNode::sendMsg Sends a string message to ControllerNode
 * @param stringMsg Message to send to ControllerNode
 */
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

/**
 * @brief DiskNode::saveFile Saves the message sent from ControllerNode in the corresponding block, and generates and stores the metadata of the file
 * @param jsonMessage json object of the message received from Controller node. It contains the name of the file and the contents, all encrypted with
 * Huffman code
 */
void DiskNode::saveFile(json jsonMessage) {
    string decodedData =  jsonMessage["Contents"];
    string fileName = jsonMessage["Name"];
    int decodedDataLen = decodedData.length();
    int initialDecodedDataLen = decodedDataLen;
    int initialStartingBit = 0;
    int initialStartingLength = 0;
    ifstream iMetadataFile;
    string metadataPath = libPath;
    metadataPath.append("/METADATA.txt");
    cout << metadataPath << endl;

    //Setting relevant metadata
    Metadata metadata(0,0,metadataPath);

    iMetadataFile.open(metadata.getFilePath());
    bool noMetadataFlag = false;
    if (iMetadataFile.is_open()){
        //Getting last element position to write in the next available spot
        iMetadataFile.seekg(0, ios::end);
        if (iMetadataFile.tellg() == 0){
            //metadata is empty, write at 0,0
            cout << "No Metadata" << endl;
            noMetadataFlag = true;
        } else{
            //metadata is not empty, get bit position and length of last message
            iMetadataFile.seekg(0, ios::beg);
            string lineFile;
            while (getline(iMetadataFile,lineFile)){}
            cout << lineFile << endl;
            string sStartBit = lineFile.substr(13,4);
            initialStartingBit = stoi(sStartBit);
            cout << "StartBit: " << sStartBit << endl;
            metadata.setStartBit(stoi(sStartBit));

            string sFileLength = lineFile.substr(22,4);
            initialStartingLength = stoi(sFileLength);
            metadata.setFileLength(stoi(sFileLength));
            cout << "FileLength: " << metadata.getFileLength() << endl;

            string sFileNum = lineFile.substr(5,4);
            metadata.setFileNum(stoi(sFileNum));
            cout << "FileNum: " << metadata.getFileNum() << endl;
            metadata.setFileNum(metadata.getFileNum()+1);
        }
        iMetadataFile.close();
        int blockNum = ((metadata.getStartBit()+metadata.getFileLength())/512)+1;
        string substring2;

        while (((512*blockNum) - (metadata.getStartBit()+metadata.getFileLength()) < decodedDataLen)){
            cout << "OVERFLOWING" << endl;
            int divisionPoint = ((512*blockNum) - (metadata.getStartBit()+metadata.getFileLength()));
            substring2 = decodedData.substr(0, divisionPoint);
            cout << substring2 << endl;
            decodedData = decodedData.substr(divisionPoint, decodedDataLen-divisionPoint);
            cout << decodedData << endl;

            string blockPath = libPath;
            blockPath.append("/BLOCK");
            string sBlockNum = to_string(blockNum);
            blockPath.append(sBlockNum);
            blockPath.append(".txt");
            cout << blockPath << endl;

            DataBlock dataBlock(substring2,blockPath);

            ifstream iBlockFile(dataBlock.getFilePath());

            if (iBlockFile.is_open()){
                string fileData;
                getline(iBlockFile,fileData);
                cout << fileData << endl;
                fileData.replace(((metadata.getStartBit()+metadata.getFileLength())%512), dataBlock.getDataString().length(), dataBlock.getDataString());
                cout << fileData << endl;
                ofstream oBlockFile(dataBlock.getFilePath(), ios::trunc);
                oBlockFile << fileData;
                oBlockFile.close();
            } else {
                perror("ERROR unable to read Block text file");
                exit(1);
            }
            blockNum++;
            decodedDataLen = decodedData.length();
            metadata.setFileLength(metadata.getFileLength()+dataBlock.getDataString().length());
        }

        substring2 = decodedData;

        string blockPath = libPath;
        blockPath.append("/BLOCK");
        string sBlockNum = to_string(blockNum);
        blockPath.append(sBlockNum);
        blockPath.append(".txt");
        cout << blockPath << endl;

        DataBlock dataBlock(substring2,blockPath);

        ifstream iBlockFile(dataBlock.getFilePath());

        if (iBlockFile.is_open()){
            string fileData;
            getline(iBlockFile,fileData);
            cout << fileData << endl;
            fileData.replace(((metadata.getStartBit()+metadata.getFileLength())%512), dataBlock.getDataString().length(), dataBlock.getDataString());
            cout << fileData << endl;
            ofstream oBlockFile(dataBlock.getFilePath(), ios::trunc);
            oBlockFile << fileData;
            oBlockFile.close();
        } else {
            perror("ERROR unable to read Block text file");
            exit(1);
        }

        //Adding new file saved to the metadata list
        ofstream oMetadataFile(metadata.getFilePath(), ios::app);
        string newMetadataLine;
        if (noMetadataFlag){
            newMetadataLine = "file:";
        } else{
            newMetadataLine = "\nfile:";
        }
        string sFileNumber = to_string(metadata.getFileNum());
        int amountZeroes = 4;
        for (int i = 10; i <= 1000; i = i*10) {
            if (metadata.getFileNum()/i > 0){
                amountZeroes--;
            } else {
                amountZeroes--;
                break;
            }
        }
        if (amountZeroes != 0){
            sFileNumber.insert(0, amountZeroes ,'0');
        }
        newMetadataLine.append(sFileNumber);
        newMetadataLine.append(" sb:");

        int sbNew = initialStartingBit+initialStartingLength;
        string sSbNew = to_string(sbNew);
        amountZeroes = 4;
        for (int i = 10; i <= 1000; i = i*10) {
            if (sbNew/i > 0){
                amountZeroes--;
            } else {
                amountZeroes--;
                break;
            }
        }
        if (amountZeroes != 0){
            sSbNew.insert(0, amountZeroes ,'0');
        }
        newMetadataLine.append(sSbNew);
        newMetadataLine.append(" len:");

        int lengthFile = initialDecodedDataLen;
        string sLengthFile = to_string(lengthFile);
        amountZeroes = 4;
        for (int i = 10; i <= 10000; i = i*10) {
            if (lengthFile/i > 0){
                amountZeroes--;
            } else {
                amountZeroes--;
                break;
            }
        }
        if (amountZeroes != 0){
            sLengthFile.insert(0, amountZeroes ,'0');
        }
        newMetadataLine.append(sLengthFile);

        string sFileName = " fileName:";
        sFileName.append(fileName);
        newMetadataLine.append(sFileName);
        cout << newMetadataLine << endl;
        //Add filename to metadata file
        oMetadataFile << newMetadataLine;

        string jsonSend = jsonMessage.dump();
        sendMsg(jsonSend);

    } else{
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }
}


/**
 * @brief DiskNode::recoverFile Sends to ControllerNode the indicated file of data.
 * @param jsonMessage json object of the message received from Controller node. It contains the number of the requested file, encrypted with
 * Huffman code.
 */
void DiskNode::recoverFile(json jsonMessage) {
    int fileNum = jsonMessage["Num"];

    ifstream iMetadataFile;
    string metadataPath = libPath;
    metadataPath.append("/METADATA.txt");
    cout << metadataPath << endl;

    //Setting relevant metadata
    Metadata metadata(0,0,metadataPath);

    iMetadataFile.open(metadata.getFilePath());
    if (iMetadataFile.is_open()) {
        iMetadataFile.seekg(0, ios::beg);
        string lineFile;
        while (getline(iMetadataFile,lineFile)){
            if(lineFile.length() == 0){
                continue;
            }
            cout << lineFile << endl;
            int currentFileNumber = stoi(lineFile.substr(5,4));
            cout << currentFileNumber << endl;
            if (currentFileNumber == fileNum) {
                cout << "File Found!" << endl;
                break;
            }
        }
        int startBit = stoi(lineFile.substr(13,4));
        int fileLength = stoi(lineFile.substr(22,4));
        cout << "StartBit: " << startBit << endl;
        cout << "FileLength: " << fileLength << endl;
        metadata.setStartBit(startBit);
        metadata.setFileLength(fileLength);
        iMetadataFile.close();

        int startBlockNum = (metadata.getStartBit()/512)+1;
        int endBlockNum = ((metadata.getStartBit()+metadata.getFileLength())/512)+1;
        DataBlock dataBlock("","");
        while ( startBlockNum != endBlockNum ){
            cout << "OVERFLOWING" << endl;

            string blockPath = libPath;
            blockPath.append("/BLOCK");
            string sBlockNum = to_string(startBlockNum);
            blockPath.append(sBlockNum);
            blockPath.append(".txt");
            cout << blockPath << endl;

            dataBlock.setFilePath(blockPath);

            ifstream iBlockFile(dataBlock.getFilePath());

            if (iBlockFile.is_open()){
                jsonMessage["IfExists"] = true;
                string fileData;
                getline(iBlockFile,fileData);
                cout << fileData << endl;
                dataBlock.setDataString(dataBlock.getDataString() + fileData.substr(metadata.getStartBit()%512,512));
                metadata.setFileLength(metadata.getFileLength() - (512 - (metadata.getStartBit() % 512)));
                metadata.setStartBit(0);
            } else {
                jsonMessage["IfExists"] = false;
            }
            startBlockNum++;
        }

        string blockPath = libPath;
        blockPath.append("/BLOCK");
        string sBlockNum = to_string(endBlockNum);
        blockPath.append(sBlockNum);
        blockPath.append(".txt");
        cout << blockPath << endl;

        dataBlock.setFilePath(blockPath);

        ifstream iBlockFile(dataBlock.getFilePath());

        if (iBlockFile.is_open()){
            jsonMessage["IfExists"] = true;
            string fileData;
            getline(iBlockFile,fileData);
            cout << fileData << endl;
            dataBlock.setDataString(dataBlock.getDataString() + fileData.substr(metadata.getStartBit()%512,metadata.getFileLength()));
            metadata.setStartBit(0);
        } else {
            jsonMessage["IfExists"] = false;
        }

        cout << dataBlock.getDataString() << endl;
        jsonMessage["Contents"] = dataBlock.getDataString();

        string jsonSend = jsonMessage.dump();
        //sendMsg(jsonSend);

    } else{
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }
}

/**
 * @brief DiskNode::recoverFileAmount Sends to ControllerNode the amount of files currently stored in the Raid System.
 * @param jsonMessage json object of the message received from Controller node.
 */
void DiskNode::recoverFileAmount(json jsonMessage) {
    int numberFiles = 0;
    ifstream iMetadataFile;
    string metadataPath = libPath;
    metadataPath.append("/METADATA.txt");
    cout << metadataPath << endl;

    iMetadataFile.open(metadataPath);
    if (iMetadataFile.is_open()) {
        iMetadataFile.seekg(0, ios::beg);
        string lineFile;
        while (getline(iMetadataFile,lineFile)){
            numberFiles++;
        }
        cout << numberFiles << endl;

        jsonMessage["Amount"] = numberFiles;

        string jsonSend = jsonMessage.dump();
        sendMsg(jsonSend);
    } else {
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }

}

/**
 * @brief DiskNode::recoverFileMetadata Sends to ControllerNode all of the metadata that correspond to the number of the file specified in jsonMessage.
 * @param jsonMessage json object of the message received from Controller node. Contains the number of the file to return the metadata of.
 */
void DiskNode::recoverFileMetadata(json jsonMessage) {
    int fileNum = jsonMessage["Num"];

    ifstream iMetadataFile;
    string metadataPath = libPath;
    metadataPath.append("/METADATA.txt");
    cout << metadataPath << endl;

    //Setting relevant metadata
    Metadata metadata(0,0,metadataPath);

    iMetadataFile.open(metadata.getFilePath());
    if (iMetadataFile.is_open()) {
        iMetadataFile.seekg(0, ios::beg);
        string lineFile;
        while (getline(iMetadataFile,lineFile)){
            if(lineFile.length() == 0){
                continue;
            }
            cout << lineFile << endl;
            int currentFileNumber = stoi(lineFile.substr(5,4));
            cout << currentFileNumber << endl;
            if (currentFileNumber == fileNum) {
                cout << "File Found!" << endl;
                break;
            }
        }
        int startBit = stoi(lineFile.substr(13,4));
        int fileLength = stoi(lineFile.substr(22,4));
        string fileName = lineFile.substr(37,string::npos);
        cout << "StartBit: " << startBit << endl;
        cout << "FileLength: " << fileLength << endl;
        cout << "FileName: " << fileName << endl;
        iMetadataFile.close();

        jsonMessage["Start bit"] = startBit;
        jsonMessage["File length"] = fileLength;
        jsonMessage["Name"] = fileName;

        string jsonSend = jsonMessage.dump();
        sendMsg(jsonSend);

    } else{
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }
}

