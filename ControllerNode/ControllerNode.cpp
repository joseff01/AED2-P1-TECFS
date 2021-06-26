#include "ControllerNode.h"
#include <stdexcept>
#include <thread>
#include "../lib/Huffman.h"

/*!
 * \brief Construct a new Controller Node:: Controller Node object
 * 
 */
ControllerNode::ControllerNode()
{
    serverSetup();
}

/*!
 * \brief Sets up the server
 * 
 */
void ControllerNode::serverSetup()
{
    int option = 1;
    struct sockaddr_in serv_addr;

    // Initialize all sockets
    for (int &i : clientSocket)
    {
        i = 0;
    }

    std::cout << "Opening Socket..." << std::endl;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNo);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    std::cout << "Binding to port " << portNo << "..." << std::endl;
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Binding Failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 6) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int addrLen = sizeof(serv_addr);
    std::cout << "Waiting for DiskNodes..." << std::endl;

    int connectionCounter = 0;
    while (connectionCounter < 7)
    {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxsd = sockfd;
        for (int i = 0; i < 7; ++i)
        {
            socketDescriptor = clientSocket[i];

            if (socketDescriptor > 0)
            {
                FD_SET(socketDescriptor, &readfds);
            }

            if (socketDescriptor > maxsd)
            {
                maxsd = socketDescriptor;
            }
        }

        activity = select(maxsd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            std::cout << "select error" << std::endl;
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            if ((newsockfd = accept(sockfd, (struct sockaddr *)&serv_addr, (socklen_t *)&addrLen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }

        printf("New connection , socket fd is %d , ip is : %s , port : %d \n", newsockfd, inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

        sendMsg(newsockfd, "Server Message Received. Connection established!");

        puts("Welcome message sent successfully");

        std::cout << receiveMsg(newsockfd) << std::endl;

        //add new client to list
        for (int i = 0; i < 7; i++)
        {
            //if position is empty
            if (clientSocket[i] == 0)
            {
                clientSocket[i] = newsockfd;
                printf("Adding to list of sockets as %d\n", i);
                connectionCounter++;
                break;
            }
        }

        for (int i = 0; i < 7; i++)
        {
            socketDescriptor = clientSocket[i];

            if (FD_ISSET(socketDescriptor, &readfds))
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read(socketDescriptor, buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(socketDescriptor, (struct sockaddr *)&serv_addr, (socklen_t *)&addrLen);
                    printf("Host disconnected , ip %s , port %d \n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close(socketDescriptor);
                    clientSocket[i] = 0;
                }
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    sendMsg(socketDescriptor, std::string(buffer));
                }
            }
        }
    }
    ////// Testing
    // storeFile("vara", "hola, este es un libro muy bonito sobre una vara muy buena!");
    // storeFile("vara2", "hola, esta es la secuela al mejor libro de sus vidas. Este es mucho más largo porque ocupo un ejemplo "
    //                     "que se bien estúpidamente largo. Aunque ocupo parar para no llenar el disco completamente.");
    std::cout << "\nBÚSQUEDAS:\n"
              << searchFiles("BLOCK") << "\n\n";
    // std::cout << searchFiles("2") << std::endl;
    // std::cout << retrieveFile("BLOCK1.txt") << std::endl;
    // std::cout << retrieveFile("BLOCK2.txt") << std::endl;
    // std::cout << retrieveFile("BLOCK3.txt") << std::endl;
    // std::cout << retrieveFile("BLOCK4.txt") << std::endl;

    //ceSearch loop
    std::thread ceSearchThread(&ControllerNode::ceSearchLoop, this);

    //ceRobot loop
    bool closeFlag = true;
    while (closeFlag)
    {
        json jsonMessage = json::parse(receiveMsg(clientSocket[5]));
        switch ((int)jsonMessage["Case"])
        {
        case CLOSE:
        {
            closeFlag = false;
            break;
        }
        case CEROBOT_REQUEST:
        {
            storeFile(jsonMessage["FileName"].get<std::string>(), jsonMessage["FileContents"].get<std::string>());
            sendMsg(clientSocket[5], jsonMessage.dump());
            break;
        }
        }
    }

    ceSearchThread.join();
}

/*!
 * \brief Loop that runs in a separate thread accepting responses from ceSEARCH
 * 
 */
void ControllerNode::ceSearchLoop()
{
    bool closeFlag = true;
    while (closeFlag)
    {
        json jsonMessage = json::parse(receiveMsg(clientSocket[6]));
        switch ((int)jsonMessage["Case"])
        {
        case CLOSE:
        {
            closeFlag = false;
            break;
        }
        case CESEARCH_LOOKUP_STR:
        {
            List<std::string> foundFiles = searchFiles(jsonMessage["FileName"].get<std::string>());
            json returnJson = {{"NamesList", foundFiles}};
            sendMsg(clientSocket[6], returnJson.dump());
            break;
        }
        case CESEARCH_GET_FILE:
        {
            std::string fileString = retrieveFile(jsonMessage["FileName"].get<std::string>());
            json returnJson = {{"Contents", fileString}};
            sendMsg(clientSocket[6], returnJson.dump());
            break;
        }
        }
    }
}

/*!
 * \brief receives a message from the socket
 * 
 * \param receiveSockfd the socket from which it'll receive a message, whether disknoedes or cerobot or cesearch
 * \return std::string with the message received
 */
std::string ControllerNode::receiveMsg(int receiveSockfd)
{
    // TODO: agregar huffman decoding y recibir 2 mensajes, el segundo tiene la info para descomprimir

    memset(buffer, 0, BUFFER_SIZE);
    int n = read(receiveSockfd, buffer, BUFFER_SIZE);
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
        int n = read(receiveSockfd, buffer, BUFFER_SIZE);
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

/*!
 * \brief Sends a message through the socket to the specified sockfd
 * 
 * \param sendSockfd 
 * \param Msg 
 */
void ControllerNode::sendMsg(int sendSockfd, std::string Msg)
{
    Huffman huff(Msg);
    LeafNode *root = huff.getDecodeTree();
    std::string encodedMsg = huff.getEncodedMsg();
    json j = *root;
    std::string treeMsg = j.dump();

    usleep(200);
    if (send(sendSockfd, encodedMsg.c_str(), strlen(encodedMsg.c_str()), 0) != strlen(encodedMsg.c_str()))
    {
        perror("send");
    }
    usleep(200);
    if (send(sendSockfd, treeMsg.c_str(), strlen(treeMsg.c_str()), 0) != strlen(treeMsg.c_str()))
    {
        perror("send");
    }
}

/*!
 * \brief Stores the file given by fileName and fileContents in the disknodes
 * 
 * \param fileName the name of the file to be stored
 * \param fileContents the contents of the file
 */
void ControllerNode::storeFile(std::string fileName, std::string fileContents)
{
    // Convert contents to binary
    std::string binContents = letters2bin(fileContents);

    List<std::string> stringDivs;

    // Dividir en 4
    int lengthFourth = binContents.length() / 4;
    for (int i = 0; i < 4; i++)
    {
        stringDivs.push_back(binContents.substr(i * lengthFourth, lengthFourth));
    }

    // if (lengthFourth > 512)
    // {
    //     std::cout << "Error: file should be less than 64 characters long";
    //     return;
    //     throw std::overflow_error("File size too big");
    // }

    // Calcular string de paridad
    std::string parityStr;
    for (int i = 0; i < lengthFourth; i++)
    {
        int p = 0;
        for (int j = 0; j < 4; j++)
        {
            p = p ^ (stringDivs[j][i] - '0');
        }
        parityStr.append(std::to_string(p));
    }
    stringDivs.push_back(parityStr);

    // stringDivs.print();

    // Saber dónde va la paridad

    // mapa
    //  0  1  2  3   4
    // [A1 A2 A3 A4 Ap] 0
    // [B1 B2 B3 Bp B4] 1
    // [C1 C2 Cp C3 C4] 2
    // [D1 Dp D2 D3 D4] 3
    // [Ep E1 E2 E3 E4] 4
    // equation to determine p disk: 4 - block#

    // Pedir cantidad de archivos
    sendMsg(clientSocket[0], json({{"Case", FILE_AMOUNT}}).dump());
    json amountResult = json::parse(receiveMsg(clientSocket[0]));

    int fileAmount = amountResult["Amount"].get<int>();

    int firstAvailableBit;

    if (fileAmount > 0)
    {
        // Revisar si va a haber overflow y si sí, mix n' match los strings
        sendMsg(clientSocket[0], json({{"Case", METADATA_FROM_NUM},
                                       {"Num", fileAmount - 1}})
                                     .dump());
        json metadataResult = json::parse(receiveMsg(clientSocket[0]));

        int lastFileSize = metadataResult["File length"].get<int>();
        firstAvailableBit = metadataResult["Start bit"].get<int>() + lastFileSize;
    }
    else
    {
        firstAvailableBit = 0;
    }

    int starterBlock = (int)firstAvailableBit / 512;
    int bitsRemainingInBlock = 512 - (firstAvailableBit % 512);
    int bitsRemainingForFile = lengthFourth;
    int currentBlock = starterBlock;
    List<std::string> finalStrings = {"", "", "", "", ""};

    while (bitsRemainingForFile > bitsRemainingInBlock)
    {
        int parityDisk = 4 - currentBlock;
        for (int disk = 0; disk < 5; disk++)
        {
            int listIndex;
            if (disk == parityDisk)
            {
                listIndex = 4;
            }
            else if (disk < parityDisk)
            {
                listIndex = disk;
            }
            else if (disk > parityDisk)
            {
                listIndex = disk - 1;
            }
            finalStrings[disk].append(stringDivs[listIndex].substr(lengthFourth - bitsRemainingForFile, bitsRemainingInBlock));
        }
        bitsRemainingForFile -= bitsRemainingInBlock;
        currentBlock++;
        bitsRemainingInBlock = 512;
    }

    // manejar el error de que se salga de la cantidad de memoria máxima, revisando si currentBlock se sale del rango
    if (currentBlock > 4)
    {
        std::cout << "ERROR: archivo intentando ser guardado excede la memoria máxima disponible en el sistema";
        return;
        throw std::overflow_error("Archivo intentando ser guardado excede la memoria máxima disponible en el sistema");
    }

    int parityDisk = 4 - currentBlock;
    for (int disk = 0; disk < 5; disk++)
    {
        int listIndex;
        if (disk == parityDisk)
        {
            listIndex = 4;
        }
        else if (disk < parityDisk)
        {
            listIndex = disk;
        }
        else if (disk > parityDisk)
        {
            listIndex = disk - 1;
        }
        finalStrings[disk].append(stringDivs[listIndex].substr(lengthFourth - bitsRemainingForFile, bitsRemainingForFile));
    }

    // mandar request final a los discos
    for (int disk = 0; disk < 5; disk++)
    {
        sendMsg(clientSocket[disk], json({{"Case", SAVE},
                                          {"Name", fileName},
                                          {"Contents", finalStrings[disk]}})
                                        .dump());
        receiveMsg(clientSocket[disk]);
    }
}

/*!
 * \brief Transforms a regular string to a string of its binary representation
 * 
 * \param str The string to be transformed
 * \return std::string the return binary string
 */
std::string ControllerNode::letters2bin(std::string str)
{
    std::string binaryStr;

    for (int i = 0; i < str.length(); ++i)
    {
        std::bitset<8> charBits(str[i]);
        binaryStr.append(charBits.to_string());
    }

    return binaryStr;
}

/*!
 * \brief Transforms a binary representation string to the appropiate regular character string
 * 
 * \param str binary string
 * \return std::string regular string
 */
std::string ControllerNode::bin2letters(std::string str)
{
    std::string charStr;

    for (int i = 0; i < str.length(); i += 8)
    {
        char c = static_cast<char>(std::stoi(str.substr(i, 8), nullptr, 2));
        charStr.append(1, c);
    }

    return charStr;
}

/*!
 * \brief Searches all the files stored for those whose name contains the searchstring as a substring
 * 
 * \param searchString the search string
 * \return List<std::string> a list containing all the full names of files that contain the substring in their names
 */
List<std::string> ControllerNode::searchFiles(std::string searchString)
{
    // Pasar searchString a lowercase
    for (auto &c : searchString)
    {
        c = tolower(c);
    }

    // Pedir cantidad de archivos
    sendMsg(clientSocket[0], json({{"Case", FILE_AMOUNT}}).dump());
    json amountResult = json::parse(receiveMsg(clientSocket[0]));

    int fileAmount = amountResult["Amount"].get<int>();

    // Request metadata of files and check name for substrings
    List<std::string> foundNames;

    for (int i = 0; i < fileAmount; i++)
    {
        sendMsg(clientSocket[0], json({{"Case", METADATA_FROM_NUM},
                                       {"Num", i}})
                                     .dump());
        json metadataResult = json::parse(receiveMsg(clientSocket[0]));

        std::string fileName = metadataResult["Name"].get<std::string>();
        std::string lowercaseName = fileName;

        // Pasar nombre a lowercase
        for (auto &c : lowercaseName)
        {
            c = tolower(c);
        }

        // Revisar si es substring
        if (lowercaseName.find(searchString) != std::string::npos)
        {
            foundNames.push_back(fileName);
        }
    }
    std::cout << "Archivos encontrados: " << foundNames << std::endl;
    return foundNames;
}

/*!
 * \brief retrieves a file from the raid specified by fileName
 * 
 * \param fileName the exact full name of the file to be fetched
 * \return std::string the contents of the file
 */
std::string ControllerNode::retrieveFile(std::string fileName)
{

    // Pedir cantidad de archivos
    sendMsg(clientSocket[0], json({{"Case", FILE_AMOUNT}}).dump());
    json amountResult = json::parse(receiveMsg(clientSocket[0]));

    int fileAmount = amountResult["Amount"].get<int>();

    // Request contents of files and check name with fileName
    List<std::string> foundNames;

    //Look for file with the name
    for (int i = 0; i < fileAmount; i++)
    {
        sendMsg(clientSocket[0], json({{"Case", METADATA_FROM_NUM},
                                       {"Num", i}})
                                     .dump());
        json metadataResult = json::parse(receiveMsg(clientSocket[0]));

        std::string receivedFileName = metadataResult["Name"].get<std::string>();

        if (fileName.compare(receivedFileName) == 0)
        {
            // This executes when file with name was found
            std::cout << "Found the file" << std::endl;

            // Get the metadata for mixing binary data into the correct final string later on, based on which disks are parity disks for each block and doing xor reconstruction if needed
            int fileSize = metadataResult["File length"].get<int>();
            int fileStartBit = metadataResult["Start bit"].get<int>();

            int starterBlock = (int)fileStartBit / 512;
            int bitsRemainingInBlock = 512 - (fileStartBit % 512);
            int bitsRemainingForFile = fileSize;
            int currentBlock = starterBlock;

            // Lists that will hold strings, receivedBinStrings receives the raw binary strings and fixedBinStrings will hold the final rearranged bin strings so all the parity blocks are on the last string and also with reconstruction done if necessary
            List<std::string> fixedBinStrings = {"", "", "", "", ""};
            List<std::string> receivedBinStrings;

            int brokenDisk = -1;
            for (int disk = 0; disk < 5; disk++)
            {
                // ask for contents of found file, for each disk
                sendMsg(clientSocket[disk], json({{"Case", FILE_FROM_NUM},
                                                  {"Num", i}})
                                                .dump());
                json fileResult = json::parse(receiveMsg(clientSocket[disk]));

                // check if disk hasn't been deleted/corrupted
                if (fileResult["IfExists"].get<bool>())
                {
                    std::string binContents = fileResult["Contents"].get<std::string>();
                    receivedBinStrings.push_back(binContents);
                }
                else
                {
                    if (brokenDisk >= 0) // if there's already another brokenDisk then there would be 2 and reconstruction would be impossible
                    {
                        std::cout << "Error: Más de un disco corrupto o faltante para el archivo que se intentó extraer.";
                        return "";
                    }
                    //if disk isn't broken just push string normally
                    receivedBinStrings.push_back("");
                    brokenDisk = disk;
                }
            }

            if (brokenDisk >= 0) // if a disk needs to be fixed
            {
                std::string rebuiltString = std::string(fileSize, '0');

                for (int disk = 0; disk < 5; disk++) // loop disks
                {
                    if (disk == brokenDisk)
                    {
                        continue;
                    }
                    for (int j = 0; j < fileSize; j++) // loop characters of string
                    {
                        int p = rebuiltString[j] - '0';            // get the int of the corresponding char representation
                        p = p ^ receivedBinStrings[disk][j] - '0'; // xor
                        rebuiltString[j] = p + '0';                // return back to char value for the number
                    }
                }

                receivedBinStrings[brokenDisk] = rebuiltString;
            }

            // Generate the final bin strings sorted correctly with the parity info on the last string and the correct order for the other strings
            while (bitsRemainingForFile > bitsRemainingInBlock)
            {
                int parityDisk = 4 - currentBlock;
                for (int disk = 0; disk < 5; disk++)
                {
                    int listIndex;
                    if (disk == parityDisk)
                    {
                        listIndex = 4;
                    }
                    else if (disk < parityDisk)
                    {
                        listIndex = disk;
                    }
                    else if (disk > parityDisk)
                    {
                        listIndex = disk - 1;
                    }
                    fixedBinStrings[listIndex].append(receivedBinStrings[disk].substr(fileSize - bitsRemainingForFile, bitsRemainingInBlock));
                }
                bitsRemainingForFile -= bitsRemainingInBlock;
                currentBlock++;
                bitsRemainingInBlock = 512;
            }
            int parityDisk = 4 - currentBlock;
            for (int disk = 0; disk < 5; disk++)
            {
                int listIndex;
                if (disk == parityDisk)
                {
                    listIndex = 4;
                }
                else if (disk < parityDisk)
                {
                    listIndex = disk;
                }
                else if (disk > parityDisk)
                {
                    listIndex = disk - 1;
                }
                fixedBinStrings[listIndex].append(receivedBinStrings[disk].substr(fileSize - bitsRemainingForFile, bitsRemainingForFile));
            }

            // Finally, concatenate strings
            std::string wholeBinString = fixedBinStrings[0] + fixedBinStrings[1] + fixedBinStrings[2] + fixedBinStrings[3];
            return bin2letters(wholeBinString);
        }
    }
    std::cout << "Error: no file found that goes by name '" << fileName << "'" << std::endl;
    return fileName;
}