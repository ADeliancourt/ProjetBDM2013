#include "bdm.h"
#include "ui_bdm.h"

BDM::BDM(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDM)
{
    ui->setupUi(this);
}

BDM::~BDM()
{
    delete ui;
}

void BDM::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
