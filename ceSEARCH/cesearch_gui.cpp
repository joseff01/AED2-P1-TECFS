#include "cesearch_gui.h"
#include "ui_cesearch_gui.h"
#include "../lib/List.h"

ceSEARCH_GUI::ceSEARCH_GUI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ceSEARCH_GUI)
{
    clientSetup();
    ui->setupUi(this);
}

ceSEARCH_GUI::~ceSEARCH_GUI()
{
    delete ui;
}

void ceSEARCH_GUI::on_pushButton_clicked()
{
    ui->listWidget->clear();
    ui->plainTextEdit->clear();
    std::string str = ui->searchBar->text().toStdString();

    json jsonMsg;
    jsonMsg["Case"] = CESEARCH_LOOKUP_STR;
    jsonMsg["FileName"] = str;

    sendMsg(jsonMsg.dump());
    json sJson = receiveJson();

    // Tomando en cuenta de que se supone que me mandan un List<string>

    List<std::string> listStr = sJson["NamesList"].get<List<std::string>>(); //given list by method

    for (int i = 0; i < listStr.length(); i++)
    {
        QString current = QString::fromStdString(listStr.at(i));
        ui->listWidget->addItem(current);
    }
}

void ceSEARCH_GUI::on_listWidget_itemClicked(QListWidgetItem *item)
{
    std::string str = item->text().toStdString();
    /*
     * ENVIAR str
     * esperar a recivir
     * std::string  stringTemp= "ahjwdagdhkawhdalwjdhalwkdhalkjwdhalkjwdhaklwdjhalkwjhdalkdjhalkjwdhakjnskdjnamwdnasklhdnhjawhdlkajwhdakjhdaksndadnaijsndiauwdnlajdnwliaudadnahwdhadwadhawduhaidhawd";

     *
     */

    //Asumiendo que recive un string enorme

    json jsonMsg;
    jsonMsg["Case"] = CESEARCH_GET_FILE;
    jsonMsg["FileName"] = str;

    sendMsg(jsonMsg.dump());
    json sJson = receiveJson();

    std::string stringTemp = sJson["Contents"].get<std::string>();
    QString qstr = QString::fromStdString(stringTemp);
    ui->plainTextEdit->setPlainText(qstr);
}

/**
 * @brief ceSEARCH_GUI::clientSetup Method in charge of setting up connection with ControllerNode app
 */
void ceSEARCH_GUI::clientSetup()
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
    if (::connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
    }
    string testMessage = receiveMsg();
    cout << testMessage << endl;
    sendMsg("Client Message Received. Connection established!");
}

/**
 * @brief ceSEARCH_GUI::receiveMsg Wait for a message to arrive from ControllerNode. When it arrives, it returns it
 * @return stringBuffer string of the message received from ControllerNode
 */
string ceSEARCH_GUI::receiveMsg()
{
    memset(buffer, 0, 1025);
    int n = read(sockfd, buffer, 1025);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    string stringBuffer(buffer);
    return stringBuffer;
}

/**
 * @brief ceSEARCH_GUI::receiveJson Wait for a message to arrive from ControllerNode. When it arrives, it parses it as a json object and returns it
 * @return jsonBuffer json object of the message received from ControllerNode
 */
json ceSEARCH_GUI::receiveJson()
{
    memset(buffer, 0, 1025);
    int n = read(sockfd, buffer, 1025);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    json jsonBuffer = json::parse(buffer);
    return jsonBuffer;
}

/**
 * @brief ceSEARCH_GUI::sendMsg Sends a string message to ControllerNode
 * @param stringMsg Message to send to ControllerNode
 */
void ceSEARCH_GUI::sendMsg(string stringMsg)
{
    memset(buffer, 0, 1025);
    strncpy(buffer, stringMsg.c_str(), 1025);
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}
