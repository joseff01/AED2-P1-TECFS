#include "cesearch_gui.h"
#include "ui_cesearch_gui.h"
#include "../lib/List.h"

ceSEARCH_GUI::ceSEARCH_GUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ceSEARCH_GUI)
{
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
    /*
     *send str to controllerNode
     *wait for answer
     *
     *
     *
     */

    // Tomando en cuenta de que se supone que me mandan un List<string>


    List<std::string> listStr; //given list by method

    for(int i = 0; i<listStr.length();i++)
    {
        QString current = QString::fromStdString(listStr.at(i));
        ui->listWidget->addItem(current);
    }


}

void ceSEARCH_GUI::on_listWidget_itemClicked(QListWidgetItem *item)
{
    std::string str =item->text().toStdString();
    /*
     * ENVIAR str
     * esperar a recivir
     * std::string  stringTemp= "ahjwdagdhkawhdalwjdhalwkdhalkjwdhalkjwdhaklwdjhalkwjhdalkdjhalkjwdhakjnskdjnamwdnasklhdnhjawhdlkajwhdakjhdaksndadnaijsndiauwdnlajdnwliaudadnahwdhadwadhawduhaidhawd";

     *
     */

    //Asumiendo que recive un string enorme

    std::string  stringTemp;
    QString qstr = QString::fromStdString(stringTemp);
    ui->plainTextEdit->setPlainText(qstr);

}
