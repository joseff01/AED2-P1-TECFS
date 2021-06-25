#ifndef CESEARCH_GUI_H
#define CESEARCH_GUI_H

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
#include <QMainWindow>
#include <QListWidgetItem>

#include "../lib/json.hpp"
#include "../lib/RequestConstants.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class ceSEARCH_GUI;
}
QT_END_NAMESPACE

using namespace std;
using json = nlohmann::json;

class ceSEARCH_GUI : public QMainWindow
{
    Q_OBJECT

public:
    ceSEARCH_GUI(QWidget *parent = nullptr);
    ~ceSEARCH_GUI();

private slots:
    void on_pushButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::ceSEARCH_GUI *ui;
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
};
#endif // CESEARCH_GUI_H
