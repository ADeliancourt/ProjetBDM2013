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
#include <QTimer>

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
    ui->dateEditVr->setDate(QDate::currentDate());
    //tableau de recherche
    modelRecherche.setQuery("select numCli,libCli,telECli,telPCli,melCli from Client");
    ui->tableViewRecherche->setModel(&modelRecherche);
    ui->tableViewRecherche->resizeColumnsToContents();
    ui->tableViewRecherche->resizeRowsToContents();
    modelRecherche.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelRecherche.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelRecherche.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelRecherche.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelRecherche.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));
    //tableau de Client récapitulatif
    modelClientRecap.setQuery("select numCli,libCli,telECli,telPCli,melCli from Client");
    ui->tableViewCli->setModel(&modelClientRecap);
    ui->tableViewCli->resizeColumnsToContents();
    ui->tableViewCli->resizeRowsToContents();
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));

    //les connexions des signaux aux slots correspondants
    connect(ui->lineEditNum,SIGNAL(textChanged(QString)),this,SLOT(rechercheClient()));
    connect(ui->lineEditNom,SIGNAL(textChanged(QString)),this,SLOT(rechercheClient()));
    connect(ui->lineEditTel,SIGNAL(textChanged(QString)),this,SLOT(rechercheClient()));
    QTimer *timer=new QTimer;
    timer->setInterval(100);
    connect (timer,SIGNAL (timeout()),this, SLOT (on_action_Location_triggered()));
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
    //Taille
    modelTaille = new QSqlRelationalTableModel(this,baseBDM);
    modelTaille->setTable("Taille");
    modelTaille->setEditStrategy(QSqlRelationalTableModel::OnRowChange);
    modelTaille->select();

    //Produit
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
    ui->tableViewStk->setModel(modelProduit);
    ui->tableViewStk->show();
    ui->tabWidgetLoc->setCurrentIndex(0);
    ui->tableViewStk->resizeColumnsToContents();

    //Client en location
    modelClientLoc.setQuery("select numCli,libCli from Client where numCli in (select numCli from Location where numStat=1)");
    ui->comboBoxCliLoc->setModel(&modelClientLoc);
    ui->comboBoxCliLoc->setModelColumn(1);

    //Client
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

    //Categorie
    modelCategorie.setQuery("select numCat,libCat from Categorie");
    ui->comboBoxCat->setModel(&modelCategorie);
    ui->comboBoxCat->setModelColumn(1);
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

            //affectation des champs aux lineEdit correspondantes
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
                //QString erreurSql=BDM.lastError();
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
       QString texteReq="select * from Client "+where+" and typCli=1";
       qDebug()<<texteReq;
       modelRecherche.setQuery(texteReq);
    }
       else
       {
           where="where typCli=1";
                   QString texteReq="select * from Client "+where;
                   qDebug()<<texteReq;
                   modelRecherche.setQuery(texteReq);
       }

}

void BDM::on_pushButtonAjoutProd_clicked()
{
    //initialisation de la variable
    QSqlRecord newProd;

    //insertion de la variable dans le modele
    modelProduit->insertRecord(-1,newProd);

    //création de la requete
    QSqlQuery maRequete("select max(numProd)+1 from Produit");
    maRequete.next();
    int numeroProdOk=maRequete.value(0).toInt();
    newProd.setValue("numProd",numeroProdOk);
}

void BDM::on_radioButtonTout_clicked()
{
    //création de la requete
    modelClientRecap.setQuery("select numCli,libCli,telECli,telPCli,melCli from Client");

    //application du modele au tableView
    ui->tableViewCli->setModel(&modelClientRecap);

    //nomination des colonne
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));

    ui->pushButtonActCli->setEnabled(false);
    ui->pushButtonDesactCli->setEnabled(false);
}

void BDM::on_radioButtonActif_clicked()
{
    //création de la requete
    modelClientRecap.setQuery("select numCli,libCli,telECli,telPCli,melCli from Client where typCli=1");

    //application du modele au tableView
    ui->tableViewCli->setModel(&modelClientRecap);

    //nomination des colonne
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));

    //disable->enable
    ui->pushButtonDesactCli->setEnabled(true);
    ui->pushButtonActCli->setEnabled(false);
}

void BDM::on_radioButtonPassif_clicked()
{
    //création de la requete
    modelClientRecap.setQuery("select numCli,libCli,telECli,telPCli,melCli from Client where typCli=2");

    //application du modele au tableView
    ui->tableViewCli->setModel(&modelClientRecap);

    //nomination des colonne
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));

    //disable->enable
    ui->pushButtonActCli->setEnabled(true);
    ui->pushButtonDesactCli->setEnabled(false);
}

void BDM::on_pushButtonSupprPanierProd_clicked()
{
    //récupération de la ligne
    int noLineSelect=ui->tableWidgetProduitLoc->currentRow();

    //suppression
    ui->tableWidgetProduitLoc->removeRow(noLineSelect);
}

void BDM::on_comboBoxCliLoc_activated()
{
    ui->pushButtonRendu->setEnabled(true);
    ui->radioButtonTotal->setEnabled(true);
    ui->radioButtonPartiel->setEnabled(true);
    //affichage de l'id
    ui->comboBoxCliLoc->setModelColumn(0);

    //preparation de la requete
    QSqlQuery req;
    req.prepare("select Location.numProd,libProd,priProd,qttLoc from Produit Natural join Location where numCli=:id and numStat=1");
        req.bindValue(":id", ui->comboBoxCliLoc->currentText().toInt());
        req.exec();
        req.first();

    //execution de la requete par le modele
    modelProdLoc.setQuery(req);

    //application du modele au tableView
    ui->tableViewProdLoc->setModel(&modelProdLoc);

    //nomination des colonnes
    modelProdLoc.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelProdLoc.setHeaderData(1, Qt::Horizontal, tr("Désignation"));
    modelProdLoc.setHeaderData(2, Qt::Horizontal, tr("Valeur"));
    modelProdLoc.setHeaderData(3, Qt::Horizontal, tr("Quantité"));

    //affichage du nom


    //redimention
    ui->tableViewProdLoc->resizeColumnsToContents();
    QSqlQuery ttl;
    ttl.prepare("select sum(qttLoc*priProd) from Location natural join Produit where numCli=:id and numStat=1");
    ttl.bindValue(":id", ui->comboBoxCliLoc->currentText().toInt());
    ttl.exec();
    ttl.first();
    double total=ttl.value(0).toDouble();
    qDebug()<<total;
    QString talcan1;
    talcan1=talcan1.number(total,'f',2);

    ui->labelTotal->setText(talcan1);
    ui->comboBoxCliLoc->setModelColumn(1);
}

void BDM::on_comboBoxCat_activated()
{
    ui->pushButtonAjoutPanierProd->setEnabled(true);
    ui->pushButtonEnregistrer->setEnabled(true);
    //affichage du nom
    ui->comboBoxCat->setModelColumn(1);

    //affichage de l'id
    ui->comboBoxCat->setModelColumn(0);

    //requete
    QSqlQuery reqProd;
    reqProd.prepare("select numProd,libProd from Produit where catProd=:id");

    //récupération de l'id dans la variable
        reqProd.bindValue(":id", ui->comboBoxCat->currentText().toInt());

        //execution
        reqProd.exec();
    modelPro.setQuery(reqProd);

    //récupétation et affichage du resultat dans la combobox adéquat
    ui->comboBoxProd->setModel(&modelPro);
    ui->comboBoxProd->setModelColumn(1);
    //réaffichage du nom
    ui->comboBoxCat->setModelColumn(1);

    QString cat=ui->comboBoxCat->currentText();
    if (cat=="Maillot")
    {//si categorie="Maillot"
    ui->comboBoxTaille->setEnabled(true);//combobox active

    modelTaille=new QSqlTableModel(this);
    modelTaille->setTable("Taille");
    modelTaille->setEditStrategy(QSqlTableModel::OnRowChange);
    modelTaille->select();
    ui->comboBoxTaille->setModel(modelTaille);
    ui->comboBoxTaille->setModelColumn(1);
    }
    else
    {//sinon
    ui->comboBoxTaille->setEnabled(false);//passive
    }
}

void BDM::on_pushButtonAjoutPanierProd_clicked()
{
    modelLocation=new QSqlTableModel(this);
    modelLocation->setTable("Location");
    modelLocation->setEditStrategy(QSqlTableModel::OnRowChange);
    modelLocation->select();
    modelLocation->setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelLocation->setHeaderData(1, Qt::Horizontal, tr("Désignation"));
    modelLocation->setHeaderData(2, Qt::Horizontal, tr("Quantité"));
    modelLocation->setHeaderData(3, Qt::Horizontal, tr("Date Départ"));
    modelLocation->setHeaderData(4, Qt::Horizontal, tr("Dat Arrivé"));
    ui->tableWidgetProduitLoc->resizeColumnsToContents();
    ui->tableWidgetProduitLoc->insertRow(ui->tableWidgetProduitLoc->rowCount());
    //numéro
    ui->comboBoxProd->setModelColumn(0);
    QTableWidgetItem * itemNmProd=new QTableWidgetItem(ui->comboBoxProd->currentText());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,0,itemNmProd);
    //Designation
    ui->comboBoxProd->setModelColumn(1);
    QTableWidgetItem * itemDesProd=new QTableWidgetItem(ui->comboBoxProd->currentText());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,1,itemDesProd);
    //Quantité
    QTableWidgetItem * itemQTTProd=new QTableWidgetItem(QString::number(ui->spinBoxQtt->value()));
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,2,itemQTTProd);
    //Taille
    QTableWidgetItem * itemTail=new QTableWidgetItem(ui->comboBoxTaille->currentText());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,3,itemTail);
    //date départ
    QTableWidgetItem * itemDD=new QTableWidgetItem(ui->dateEditDepart->text());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,4,itemDD);
    //date arrivé
    QTableWidgetItem * itemDA=new QTableWidgetItem(ui->dateEditArrive->text());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,5,itemDA);
}

void BDM::on_pushButtonActCli_clicked()
{
    QModelIndex index=ui->tableViewCli->selectionModel()->currentIndex();
    int noLigne=ui->tableViewCli->selectionModel()->currentIndex().row();
    index=index.sibling(noLigne,0);
    QVariant numeroClient=index.data().toInt();
    qDebug()<<numeroClient;
    QSqlQuery reqDesact;
    reqDesact.prepare("update Client set typCli=1 where numCli=:id");
        reqDesact.bindValue(":id", numeroClient);
        reqDesact.exec();
    //application du modele à la tableView
    ui->tableViewCli->setModel(&modelClientRecap);
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));

}

void BDM::on_pushButtonDesactCli_clicked()
{
    QModelIndex index=ui->tableViewCli->selectionModel()->currentIndex();
    int noLigne=ui->tableViewCli->selectionModel()->currentIndex().row();
    index=index.sibling(noLigne,0);
    QVariant numeroClient=index.data().toInt();
    //ui->comboBoxCli->setModelColumn(0);
    //nouvelle version
    //preparation de la requete
    QSqlQuery reqDesact;
    reqDesact.prepare("update Client set typCli=2 where numCli=:id");
        reqDesact.bindValue(":id", numeroClient);
        reqDesact.exec();

    ui->tableViewCli->setModel(&modelClientRecap);
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));
}

void BDM::on_pushButtonEnregistrer_clicked()
{
    QModelIndex index=ui->tableViewRecherche->selectionModel()->currentIndex();
    int noLigne=ui->tableViewRecherche->selectionModel()->currentIndex().row();
    index=index.sibling(noLigne,0);
    QVariant numeroClient=index.data().toInt();
    for(int noLine=0;noLine<ui->tableWidgetProduitLoc->rowCount();noLine++)
    {
        int qtt=ui->tableWidgetProduitLoc->item(noLine,2)->text().toInt();
        int prod=ui->tableWidgetProduitLoc->item(noLine,0)->text().toInt();
        QSqlQuery alerte;
        alerte.prepare("Select altProd from Produit where numProd=:idProd");
        alerte.bindValue(":idProd", prod);
        alerte.exec();
        int alt=alerte.value(0).toInt();
        if(qtt<alt)
        {

            QString dep=ui->dateEditDepart->text();
            QString arr=ui->dateEditArrive->text();
            QString reel="00-00-0000";
            QSqlQuery reqLoc;
            reqLoc.prepare("INSERT INTO Location values (:idCli,:idProd,:qtt,\""+dep+"\",\""+arr+"\",\""+reel+"\",:stat)");
            reqLoc.bindValue(":idCli", numeroClient);
            reqLoc.bindValue(":idProd", prod);
            reqLoc.bindValue(":qtt", qtt);
            reqLoc.bindValue(":stat", 1);
            reqLoc.exec();
            int prodi=ui->tableWidgetProduitLoc->item(noLigne,0)->text().toInt();
            int qtti=ui->tableWidgetProduitLoc->item(noLigne,2)->text().toInt();
            QSqlQuery decompte;
            decompte.prepare("update Produit set stkReel=stkReel-:qtt where numProd=:idProd");
            decompte.bindValue(":qtt", qtti);
            decompte.bindValue(":idProd", prodi);
            decompte.exec();
      }}
    QMessageBox::warning(this,this->windowTitle(),"L'opération a bien été\renregistrée.");
    preparationLoc();

}

void BDM::preparationLoc()
{
    //cette procédure sert à vider tout les champs pour préparer une nouvelle commande
    ui->lineEditNum->clear();
    ui->lineEditNom->clear();
    ui->lineEditTel->clear();

    //suppression des items et du texte de la table widget
    ui->tableWidgetProduitLoc->setRowCount(0);
}

void BDM::on_pushButtonRendu_clicked()
{
    ui->comboBoxCliLoc->setModelColumn(0);
    int numeroCli=ui->comboBoxCliLoc->currentText().toInt();
    QModelIndex index=ui->tableViewProdLoc->selectionModel()->currentIndex();
    int noLigne=ui->tableViewProdLoc->selectionModel()->currentIndex().row();
    index=index.sibling(noLigne,0);
    QVariant numeroProd=index.data().toInt();
    if(numeroProd!=0)
    {
        QString Vr=ui->dateEditVr->text();
        QModelIndex index=ui->tableViewProdLoc->selectionModel()->currentIndex();
        int maxi=ui->tableViewProdLoc->selectionModel()->currentIndex().row();
        index=index.sibling(maxi,3);
        QVariant num=index.data();
        int max=num.value<int>();
        QSqlQuery Date;
        Date.prepare("update Location set dateArrReel=\""+Vr+"\" where numProd=:idProd and numCli=:idCli");
        Date.bindValue(":idProd", numeroProd);
        Date.bindValue(":idCli", numeroCli);
        Date.exec();
        int qtt=ui->spinBoxRendu->value();
        qDebug()<<qtt;
        QSqlQuery rendu;
        rendu.prepare("update Produit set stkReel=stkReel+:qtt where numProd=:idProd");
        rendu.bindValue(":qtt", qtt);
        rendu.bindValue(":idProd", numeroProd);
        rendu.exec();
        if(qtt==max)
        {
            qDebug()<<numeroCli;
            qDebug()<<numeroProd;
            QSqlQuery stat;
            stat.prepare("update Location set numStat=2 where numProd=:idProd and numCli=:idCli");
            stat.bindValue(":idProd", numeroProd);
            stat.bindValue(":idCli", numeroCli);
            stat.exec();
            ui->comboBoxCliLoc->setModelColumn(1);
        }
        else
        {
            ui->comboBoxCliLoc->setModelColumn(0);
            int numeroCli=ui->comboBoxCliLoc->currentText().toInt();
            int qtti=ui->spinBoxRendu->value();
            QSqlQuery decompte;
            decompte.prepare("update Location set qttLoc=qttLoc-:qtt where numProd=:idProd and numCli=:idCli");
            decompte.bindValue(":qtt", qtti);
            decompte.bindValue(":idCli", numeroCli);
            decompte.bindValue(":idProd", numeroProd);
            decompte.exec();
            ui->comboBoxCliLoc->setModelColumn(1);

        }
        QMessageBox::warning(this,this->windowTitle(),"L'opération a bien été\renregistrée.");
    }
    else
    {

    }
}

void BDM::on_radioButtonPartiel_clicked()
{
    ui->spinBoxRendu->setEnabled(true);
}

void BDM::on_radioButtonTotal_clicked()
{
    ui->spinBoxRendu->setEnabled(false);
}

void BDM::on_tableViewProdLoc_clicked()
{
    QModelIndex index=ui->tableViewProdLoc->selectionModel()->currentIndex();
    int maxi=ui->tableViewProdLoc->selectionModel()->currentIndex().row();
    index=index.sibling(maxi,3);
    QVariant num=index.data();
    int max=num.value<int>();
    ui->spinBoxRendu->setValue(max);
}
