#ifndef CESEARCH_GUI_H
#define CESEARCH_GUI_H

#include <QMainWindow>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class ceSEARCH_GUI; }
QT_END_NAMESPACE

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
};
#endif // CESEARCH_GUI_H
