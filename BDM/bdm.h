#ifndef BDM_H
#define BDM_H

#include <QMainWindow>

namespace Ui {
    class BDM;
}

class BDM : public QMainWindow {
    Q_OBJECT
public:
    BDM(QWidget *parent = 0);
    ~BDM();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::BDM *ui;
};

#endif // BDM_H
