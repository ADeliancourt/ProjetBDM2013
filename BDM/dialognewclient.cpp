#include "dialognewclient.h"
#include "ui_dialognewclient.h"
#include "QSqlQuery"

DialogNewClient::DialogNewClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewClient)
{
    ui->setupUi(this);
    QSqlQuery req("Select ifnull (max(numCli)+1,1) from Client");
    req.first();
    QString numCliMax=req.value(0).toString();
    ui->lineEditIDNew->setText(numCliMax);
}

DialogNewClient::~DialogNewClient()
{
    delete ui;
}
