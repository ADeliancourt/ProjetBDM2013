#ifndef DIALOGNEWCLIENT_H
#define DIALOGNEWCLIENT_H

#include <QDialog>

namespace Ui {
class DialogNewClient;
}

class DialogNewClient : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogNewClient(QWidget *parent = 0);
    ~DialogNewClient();
    Ui::DialogNewClient *ui;
    
private:
};

#endif // DIALOGNEWCLIENT_H
