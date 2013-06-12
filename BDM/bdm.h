#ifndef BDM_H
#define BDM_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QComboBox>

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

private slots:
    void on_action_Quitter_triggered();

    void on_action_Location_triggered();

    void on_pushButtonNewCli_clicked();

    void rechercheClient();

    void on_pushButtonAjoutProd_clicked();

    void on_radioButtonTout_clicked();

    void on_radioButtonActif_clicked();

    void on_radioButtonPassif_clicked();

    void on_pushButtonDesactCli_clicked();

    void on_pushButtonActCli_clicked();

    void on_pushButtonAjoutPanierProd_clicked();

    void on_pushButtonSupprPanierProd_clicked();

    void on_pushButtonEnregistrer_clicked();

    void on_pushButtonRendu_clicked();

    void on_comboBoxCliLoc_activated(const QString &arg1);

    void on_comboBoxCat_activated(const QString &arg1);

private:
    Ui::BDM *ui;
    QSqlTableModel * modelLocation;
    QSqlTableModel * modelProduit;
    QSqlTableModel * modelTaille;
    QSqlTableModel * modelClient;
    QSqlQueryModel   modelCategorie;
    QSqlQueryModel   modelPro;
    QSqlQueryModel   modelClientLoc;
    QSqlQueryModel   modelProdLoc;
    QSqlQueryModel   modelClientRecap;
    QSqlQueryModel   modelRecherche;
    QSqlDatabase     baseBDM;
};

#endif // BDM_H
