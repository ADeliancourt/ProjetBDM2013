#include "bdm.h"
#include "ui_bdm.h"
#include <QMessageBox>
#include <QTextCodec>
#include <QSqlRelationalTableModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDate>
#include <QDebug>
#include "dialognewclient.h"
#include "ui_dialognewclient.h"
#include <QSqlError>
#include <QScrollArea>

BDM::BDM(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDM)
{
    ui->setupUi(this);    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //connexion
    baseBDM=QSqlDatabase::addDatabase("QMYSQL");
    baseBDM.setHostName("localhost");
    baseBDM.setUserName("root");
    baseBDM.setPassword("ar3f9b");
    baseBDM.setDatabaseName("BDM");
    if(!baseBDM.open())
    {
        QMessageBox::warning(this,this->windowTitle(),"Le serveur de base de données est inaccessible.\r\rMerci de contacter votre administrateur et de relancer l'application après avoir réglé le problème.");
    }
    else
    {
    ui->statusBar->setStatusTip("Connexion à la base réussite");
    on_action_Location_triggered();
    }
    ui->dateEditDepart->setDate(QDate::currentDate());
    ui->dateEditArrive->setDate(QDate::currentDate());

    //tableau de recherche
    modelRecherche.setQuery("select libCli,telECli,telPCli,melCli from Client");
    //ui->tableViewRecherche->setModel(&modelRecherche);
    ui->tableViewRecherche->resizeColumnsToContents();
    ui->tableViewRecherche->resizeRowsToContents();
    modelRecherche.setHeaderData(0, Qt::Horizontal, tr("Société"));
    modelRecherche.setHeaderData(1, Qt::Horizontal, tr("Téléphone fixe"));
    modelRecherche.setHeaderData(2, Qt::Horizontal, tr("Portable"));
    modelRecherche.setHeaderData(3, Qt::Horizontal, tr("Adresse mail"));

    //tableau de Client passif
    modelClientPassif.setQuery("select libCli,telECli,telPCli,melCli from Client where typCli=2");
    //ui->tableViewCliPassif->setModel(&modelClientPassif);
    ui->tableViewCliPassif->resizeColumnsToContents();
    ui->tableViewCliPassif->resizeRowsToContents();
    modelClientPassif.setHeaderData(0, Qt::Horizontal, tr("Société"));
    modelClientPassif.setHeaderData(1, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientPassif.setHeaderData(2, Qt::Horizontal, tr("Portable"));
    modelClientPassif.setHeaderData(3, Qt::Horizontal, tr("Adresse mail"));

    //les connexions des signaux aux slots correspondants
    connect(ui->lineEditNum,SIGNAL(textChanged(QString)),this,SLOT(rechercheClient()));
    connect(ui->lineEditNom,SIGNAL(textChanged(QString)),this,SLOT(rechercheClient()));
    connect(ui->lineEditTel,SIGNAL(textChanged(QString)),this,SLOT(rechercheClient()));
    //ui->comboBoxCat->setModel(modelCategorie);
    ui->comboBoxCat->setModelColumn(1);
    QString talcan=ui->comboBoxCat->currentText();
  /*  if(talcan=="Literie")
{*/
    QString reqProd = "select libProd from Produit inner join Categorie on catProd=numCat where libCat=\""+talcan+"\"";
    // exécution de la requête
    QSqlQuery reqExProd(reqProd,baseBDM);
    // compteur de lignes
    int i = 0;
    reqExProd.exec();
    // boucle pour remplir ma comboBox
    while(reqExProd.next())
    {
        //ui->comboBoxProd->addItem(reqExProd.value(0).toString(),i);
        i++;
    }
/*}
   if(talcan=="Eponge")
    {
        QString reqProd = "select libProd from Produit inner join Categorie on catProd=numCat where libCat=\"Eponge\"";
        // exécution de la requête
        QSqlQuery reqExProd(reqProd,baseBDM);
        // compteur de lignes
        int i = 0;
        reqExProd.exec();
        // boucle pour remplir ma comboBox
        while(reqExProd.next())
        {
            ui->comboBoxProd->addItem(reqExProd.value(0).toString(),i);
            i++;
        }
    }*/

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

void BDM::on_action_Quitter_triggered()
{
    if(QMessageBox::question(this,this->windowTitle(),tr("Êtes-vous sûr de vouloir quitter l'application ?"),QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel)==QMessageBox::Ok)
    {
        close();
    }
}

void BDM::on_action_Location_triggered()
{
    modelCategorie = new QSqlRelationalTableModel(this,baseBDM);
    modelCategorie->setTable("Categorie");
    modelCategorie->setEditStrategy(QSqlRelationalTableModel::OnRowChange);
    modelCategorie->select();
    modelTaille = new QSqlRelationalTableModel(this,baseBDM);
    modelTaille->setTable("Taille");
    modelTaille->setEditStrategy(QSqlRelationalTableModel::OnRowChange);
    modelTaille->select();

    modelProduit = new QSqlRelationalTableModel(this,baseBDM);
    modelProduit->setTable("Produit");
    modelProduit->setEditStrategy(QSqlRelationalTableModel::OnRowChange);
    modelProduit->select();
    modelProduit->setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelProduit->setHeaderData(1, Qt::Horizontal, tr("Designation"));
    modelProduit->setHeaderData(2, Qt::Horizontal, tr("Total"));
    modelProduit->setHeaderData(3, Qt::Horizontal, tr("En stock"));
    modelProduit->setHeaderData(4, Qt::Horizontal, tr("Alerte"));
    modelProduit->setHeaderData(5, Qt::Horizontal, tr("Prix"));
    modelProduit->setHeaderData(6, Qt::Horizontal, tr("Catégorie"));
    modelProduit->setHeaderData(7, Qt::Horizontal, tr("Taille"));
    //ui->tableViewStk->setModel(modelProduit);
    ui->tableViewStk->show();
    ui->tabWidgetLoc->setCurrentIndex(0);
    ui->tableViewStk->resizeColumnsToContents();

    //Client Actif
    modelClient = new QSqlRelationalTableModel(this,baseBDM);
    modelClient->setTable("Client");
    modelClient->setEditStrategy(QSqlRelationalTableModel::OnRowChange);
    modelClient->select();
    modelClient->setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClient->setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClient->setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClient->setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClient->setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));
    modelClient->setHeaderData(5, Qt::Horizontal, tr("Type"));
    //ui->tableViewCliActif->setModel(modelClient);
    ui->tableViewCliActif->show();
    ui->tabWidgetLoc->setCurrentIndex(0);
    ui->tableViewCliActif->resizeColumnsToContents();
    QString talcan=ui->comboBoxCat->currentText();
    if(talcan=="Maillot")
    {
    ui->comboBoxTaille->setEnabled(true);
    }
   // ui->comboBoxTaille->setModel(modelTaille);
    ui->comboBoxTaille->setModelColumn(1);
}

void BDM::on_pushButtonNewCli_clicked()
{
    //ouverture de la popup "Nouveau client"
        DialogNewClient * formNewCli=new DialogNewClient(this);

        if(formNewCli->exec()==QDialog::Accepted)
        {
            qDebug()<<"Ajout d'un nouveau client";
            //enregistrement du newCli
            QSqlRecord recordClient=modelClient->record();

            recordClient.setValue("numCli",formNewCli->ui->lineEditIDNew->text().toInt());
            recordClient.setValue("libCli",formNewCli->ui->lineEditNomNew->text());
            recordClient.setValue("melCli",formNewCli->ui->lineEditMelNew->text());
            recordClient.setValue("telECli",formNewCli->ui->lineEditTelFNew->text());
            recordClient.setValue("telPCli",formNewCli->ui->lineEditTelPNew->text());
            if (modelClient->insertRecord(-1,recordClient))
            {
                ui->statusBar->showMessage("Client ajouté avec succés",3000);
                modelClient->submit();
                ui->lineEditNum->setText(formNewCli->ui->lineEditIDNew->text());
                ui->lineEditNom->setText(formNewCli->ui->lineEditNomNew->text());
                ui->lineEditTel->setText(formNewCli->ui->lineEditTelFNew->text());
            }
            else
            {
                //QString erreurSql=Panier.lastError().
            }
        }
        else
        {
            qDebug()<<"Pas d'ajout";
        }
}
void BDM::rechercheClient()
{
    QString where="";
    QStringList listeRestrictions;
    //ID
    if(ui->lineEditNum->text()!="")
    {
        listeRestrictions.append(" numCli like '%"+ui->lineEditNum->text()+"%'");
    }
    //Nom
    if(ui->lineEditNom->text()!="")
    {
        listeRestrictions.append(" libCli like '%"+ui->lineEditNom->text()+"%'");
    }
    //Prenom
    if(ui->lineEditTel->text()!="")
    {
        listeRestrictions.append(" telECli like '%"+ui->lineEditTel->text()+"%'");
    }
       if(listeRestrictions.count()>0)
    {
        where="where "+listeRestrictions.join(" and ");

    }
       QString texteReq="select * from Client "+where+" and typCli=1";
    qDebug()<<texteReq;
    modelRecherche.setQuery(texteReq);
}


void BDM::on_pushButtonAjoutProd_clicked()
{
    QSqlRecord newProd;
    modelProduit->insertRecord(-1,newProd);
    QSqlQuery maRequete("select max(numProd)+1 from Produit");
    maRequete.next();
    int numeroProdOk=maRequete.value(0).toInt();
    newProd.setValue("numProd",numeroProdOk);
}

void BDM::on_pushButtonDesactCli_clicked()
{

}

void BDM::on_pushButtonActCli_clicked()
{

}
