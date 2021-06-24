#include "ControllerNode.h"

ControllerNode::ControllerNode() {
    serverSetup();
}

void ControllerNode::serverSetup() {
    int option = 1;
    struct sockaddr_in serv_addr;

    // Initialize all sockets
    for (int & i : clientSocket) {
        i = 0;
    }

    cout << "Opening Socket..." << endl;
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
    cout << "Binding to port " << portNo << "..." << endl;
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Binding Failed");
        exit(EXIT_FAILURE);
    }

    if(listen(sockfd, 6) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int addrLen = sizeof(serv_addr);
    cout << "Waiting for DiskNodes..." << endl;

    int connectionCounter = 0;
    while (connectionCounter < 5){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxsd = sockfd;
        for (int i = 0; i < 7; ++i) {
            socketDescriptor = clientSocket[i];

            if (socketDescriptor > 0){
                FD_SET(socketDescriptor,&readfds);
            }

            if (socketDescriptor > maxsd){
                maxsd = socketDescriptor;
            }
        }

        activity = select( maxsd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)){
            cout << "select error" << endl;
        }

        if (FD_ISSET(sockfd, &readfds)){
            if ((newsockfd = accept(sockfd,(struct sockaddr *)&serv_addr, (socklen_t*)&addrLen)) < 0){
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }

        printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , newsockfd , inet_ntoa(serv_addr.sin_addr) , ntohs(serv_addr.sin_port));

        if( send(newsockfd, "Server Message Received. Connection established!", strlen("Server Message Received. Connection established!"), 0) != strlen("Server Message Received. Connection established!") )
        {
            perror("send");
        }

        puts("Welcome message sent successfully");

        cout << receiveMsg(newsockfd) << endl;

        //add new client to list
        for (int i = 0; i < 7; i++){
            //if position is empty
            if( clientSocket[i] == 0 )
            {
                clientSocket[i] = newsockfd;
                printf("Adding to list of sockets as %d\n" , i);
                connectionCounter++;
                break;
            }
        }

        for (int i = 0; i < 7; i++){
            socketDescriptor = clientSocket[i];

            if (FD_ISSET( socketDescriptor , &readfds))
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( socketDescriptor , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(socketDescriptor , (struct sockaddr*)&serv_addr , (socklen_t*)&addrLen);
                    printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(serv_addr.sin_addr) , ntohs(serv_addr.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( socketDescriptor );
                    clientSocket[i] = 0;
                }
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    send(socketDescriptor , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
}

string ControllerNode::receiveMsg(int receiveSockfd) {
    memset(buffer, 0, 1025);
    int n = read(receiveSockfd,buffer,1025);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    string receivedMsg = string(buffer);
    return receivedMsg;
}

void ControllerNode::sendMsg(int sendSockfd, string Msg) {
    if( send(sendSockfd, Msg.c_str(), strlen(Msg.c_str()), 0) != strlen(Msg.c_str()) )
    {
        perror("send");
    }
}
