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
    /*
    bool closeFlag = true;
    while (closeFlag){
        json jsonMessage = receiveJson();
        switch ((int)jsonMessage["Case"]) {
            case 0:{
                closeFlag = false;
                break;
            }
            case 1:{
                saveFile(jsonMessage);
                break;
            }
            case 2:{
                recoverBlock(jsonMessage);
                break;
            }
            case 3:{
                recoverFile(jsonMessage);
                break;
            }
        }
    }
     */
    //TESTS REMOVE LATER
    json jsonExample;
    //saveFile(jsonExample);
    //recoverFileAmount(jsonExample);
    //recoverFile(jsonExample);
    recoverFileMetadata(jsonExample);
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

void DiskNode::saveFile(json jsonMessage) {
    //string encodedData = jsonMessage["Data"];
    //int fileNumber = jsonMessage["File"];
    /*
     *
     * HUFFMAN DECODING+
     *
     *
     */
    string decodedData = "forthMessage"; //CHANGE LATER
    string fileName = "pichulafelizcuatro"; //CHANGE LATER
    int decodedDataLen = decodedData.length();
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

            string sStartBit = lineFile.substr(13,4);
            cout << "StartBit: " << sStartBit << endl;
            metadata.setStartBit(stoi(sStartBit));

            string sFileLength = lineFile.substr(22,4);
            metadata.setFileLength(stoi(sFileLength));
            cout << "FileLength: " << metadata.getFileLength() << endl;

            string sFileNum = lineFile.substr(5,4);
            metadata.setFileNum(stoi(sFileNum));
            cout << "FileNum: " << metadata.getFileNum() << endl;
            metadata.setFileNum(metadata.getFileNum()+1);
        }
        iMetadataFile.close();
        int blockNum = ((metadata.getStartBit()+metadata.getFileLength())/512)+1;
        //Check if block overflows
        string substring2;
        bool overflowFlag = false;
        if(((512*blockNum) - ((metadata.getStartBit()%512)+metadata.getFileLength())) < decodedDataLen){
            cout << "OVERFLOWING" << endl;
            int divisionPoint = (512 - ((metadata.getStartBit()%512)+metadata.getFileLength()));
            substring2 = decodedData.substr(divisionPoint, decodedDataLen-divisionPoint);
            cout << substring2 << endl;
            decodedData = decodedData.substr(0, divisionPoint);
            cout << decodedData << endl;
            overflowFlag = true;
        }

        string blockPath = libPath;
        blockPath.append("/BLOCK");
        string sBlockNum = to_string(blockNum);
        blockPath.append(sBlockNum);
        blockPath.append(".txt");
        cout << blockPath << endl;

        //Instantiating relevant DataBlock
        DataBlock dataBlock(decodedData,blockPath);

        //Getting values at the block file and replacing the needed bits
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
        // If message overflows, set the second part on the next block
        if (overflowFlag){
            string blockPathOverflow = libPath;
            blockPathOverflow.append("/BLOCK");
            string sBlockNumOverflow = to_string(blockNum+1);
            blockPathOverflow.append(sBlockNumOverflow);
            blockPathOverflow.append(".txt");
            cout << blockPathOverflow << endl;

            //Instantiating next DataBlock
            DataBlock dataBlockOverflow(substring2,blockPathOverflow);

            ifstream iBlockFileOverflow(dataBlockOverflow.getFilePath());
            if (iBlockFileOverflow.is_open()){
                string fileData;
                getline(iBlockFileOverflow,fileData);
                cout << fileData << endl;
                fileData.replace(0, dataBlockOverflow.getDataString().length(), dataBlockOverflow.getDataString());
                cout << fileData << endl;
                ofstream oBlockFile(dataBlockOverflow.getFilePath(), ios::trunc);
                oBlockFile << fileData;
                oBlockFile.close();
            } else {
                perror("ERROR unable to read Overflow Block text file");
                exit(1);
            }

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
        for (int i = 10; i < 1000; i = i*10) {
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

        int sbNew = metadata.getFileLength()+metadata.getStartBit();
        string sSbNew = to_string(sbNew);
        amountZeroes = 4;
        for (int i = 10; i < 1000; i = i*10) {
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

        int lengthFile = decodedDataLen;
        string sLengthFile = to_string(lengthFile);
        amountZeroes = 4;
        for (int i = 10; i < 1000; i = i*10) {
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

        /*
         *
         *
         *
         *
         *
         * HUFFMAN ENCODING AND SEND MESSAGE
         * JSON Message to send to controller node PENDING
         *
         *
         *
         *
         */

    } else{
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }
}

void DiskNode::recoverBlock(json jsonMessage) {
    /*
     *
     * HUFFMAN DECODING+
     *
     *
     */

    int blockNum = 1; //Change Later

    string blockPath = libPath;
    blockPath.append("/BLOCK");
    string sBlockNum = to_string(blockNum);
    blockPath.append(sBlockNum);
    blockPath.append(".txt");
    cout << blockPath << endl;

    //Instantiating relevant DataBlock
    DataBlock dataBlock("",blockPath);

    //Getting values at the block file and replacing the needed bits
    ifstream iBlockFile(dataBlock.getFilePath());
    if (iBlockFile.is_open()){
        string fileData;
        getline(iBlockFile,fileData);
        iBlockFile.close();
        cout << fileData << endl;
        dataBlock.setDataString(fileData);
    } else {
        perror("ERROR unable to read Block text file");
        exit(1);
    }
    /*
         *
         *
         *
         *
         *
         * HUFFMAN ENCODING AND SEND MESSAGE
         * JSON Message to send to controller node PENDING
         *
         *
         *
         *
         */
}

void DiskNode::recoverFile(json jsonMessage) {
    /*
     *
     * HUFFMAN DECODING+
     *
     *
     */
    int fileNum = 1; //Change Later

    ifstream iMetadataFile;
    string metadataPath = libPath;
    metadataPath.append("/METADATA.txt");
    cout << metadataPath << endl;

    //Setting relevant metadata
    Metadata metadata(0,0,metadataPath);

    iMetadataFile.open(metadata.getFilePath());
    if (iMetadataFile.is_open()) {
        iMetadataFile.seekg(0, ios::end);
        if (iMetadataFile.tellg() == 0) {





            //MESSAGE ERROR TO CONTROLLER NODE PENDING!!!!!!!!!





            return;
        }
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
        //Check if file overflows to another block
        bool overflowFlag = false;
        if(startBlockNum != endBlockNum){
            cout << "File Overflows" << endl;
            overflowFlag = true;
        }

        string blockPath = libPath;
        blockPath.append("/BLOCK");
        string sBlockNum = to_string(startBlockNum);
        blockPath.append(sBlockNum);
        blockPath.append(".txt");
        cout << blockPath << endl;

        //Instantiating relevant DataBlock
        DataBlock dataBlock("", blockPath);

        //Getting values at the block file
        ifstream iBlockFile(dataBlock.getFilePath());
        if (iBlockFile.is_open()){
            string fileData;
            getline(iBlockFile,fileData);
            cout << fileData << endl;
            if (overflowFlag){
                dataBlock.setDataString(fileData.substr(metadata.getStartBit()%512,512));
            } else {
                dataBlock.setDataString(fileData.substr(metadata.getStartBit()%512,(metadata.getFileLength())%512));
            }
            cout << dataBlock.getDataString() << endl;
        } else {
            perror("ERROR unable to read Block text file");
            exit(1);
        }
        // If message overflows, get second part of file
        if (overflowFlag){
            string blockPathOverflow = libPath;
            blockPathOverflow.append("/BLOCK");
            string sBlockNumOverflow = to_string(endBlockNum);
            blockPathOverflow.append(sBlockNumOverflow);
            blockPathOverflow.append(".txt");
            cout << blockPathOverflow << endl;

            //Instantiating next DataBlock
            DataBlock dataBlockOverflow("",blockPathOverflow);
            ifstream iBlockFileOverflow(dataBlockOverflow.getFilePath());
            if (iBlockFileOverflow.is_open()){
                string fileData;
                getline(iBlockFileOverflow,fileData);
                cout << fileData << endl;
                dataBlockOverflow.setDataString(fileData.substr(0,(metadata.getStartBit()+metadata.getFileLength())%512));
                cout << dataBlockOverflow.getDataString() << endl;
            } else {
                perror("ERROR unable to read Overflow Block text file");
                exit(1);
            }
            dataBlock.setDataString(dataBlock.getDataString() + dataBlockOverflow.getDataString());
        }
        cout << dataBlock.getDataString() << endl;
        /*
          *
          *
          *
          *
          *
          * HUFFMAN ENCODING AND SEND MESSAGE
          * JSON Message to send to controller node PENDING
          *
          *
          *
          *
          */
    } else{
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }
}

void DiskNode::recoverFileAmount(json jsonMessage) {
    /*
    *
    * HUFFMAN DECODING+
    *
    *
    */

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
        /*
         *
         *
         * SEND numberFiles to controller node
         *
         *
         */
    } else {
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }

}

void DiskNode::recoverFileMetadata(json jsonMessage) {
    /*
     *
     * HUFFMAN DECODING+
     *
     *
     */
    int fileNum = 2; //Change Later

    ifstream iMetadataFile;
    string metadataPath = libPath;
    metadataPath.append("/METADATA.txt");
    cout << metadataPath << endl;

    //Setting relevant metadata
    Metadata metadata(0,0,metadataPath);

    iMetadataFile.open(metadata.getFilePath());
    if (iMetadataFile.is_open()) {
        iMetadataFile.seekg(0, ios::end);
        if (iMetadataFile.tellg() == 0) {





            //MESSAGE ERROR TO CONTROLLER NODE PENDING!!!!!!!!!





            return;
        }
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
        string fileName = lineFile.substr(36,string::npos);
        cout << "StartBit: " << startBit << endl;
        cout << "FileLength: " << fileLength << endl;
        cout << "FileName: " << fileName << endl;
        iMetadataFile.close();


        /*
          *
          *
          *
          *
          *
          * HUFFMAN ENCODING AND SEND MESSAGE
          * JSON Message to send to controller node PENDING
          *
          *
          *
          *
          */
    } else{
        perror("ERROR unable to read METADATA.txt");
        exit(1);
    }
}

