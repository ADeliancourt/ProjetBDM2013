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
    if (ui->dateEditArrive>ui->dateEditDepart)
    {
    ui->dateEditArrive=ui->dateEditDepart;
    }
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
    modelClientLoc.setQuery("select numCli,libCli from Client where numCli in (select numCli from Location)");
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

    }
       QString texteReq="select * from Client "+where+" and typCli=1";
    qDebug()<<texteReq;
    modelRecherche.setQuery(texteReq);
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

        ui->comboBoxCli->setEnabled(false);
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
    ui->comboBoxCli->setEnabled(true);
    ui->pushButtonDesactCli->setEnabled(true);
    ui->pushButtonActCli->setEnabled(false);
    ui->comboBoxCli->setModel(&modelClientRecap);
    ui->comboBoxCli->setModelColumn(1);
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
    ui->comboBoxCli->setEnabled(true);
    ui->pushButtonActCli->setEnabled(true);
    ui->pushButtonDesactCli->setEnabled(false);
    ui->comboBoxCli->setModel(&modelClientRecap);
    ui->comboBoxCli->setModelColumn(1);
}

void BDM::on_pushButtonSupprPanierProd_clicked()
{
    //récupération de la ligne
    int noLineSelect=ui->tableWidgetProduitLoc->currentRow();

    //suppression
    ui->tableWidgetProduitLoc->removeRow(noLineSelect);
}

void BDM::on_comboBoxCliLoc_activated(const QString &arg1)
{
    //affichage de l'id
    ui->comboBoxCliLoc->setModelColumn(0);

    //preparation de la requete
    QSqlQuery req;
    req.prepare("select Location.numProd,libProd,priProd from Produit Natural join Location where numCli=:id");
        req.bindValue(":id", ui->comboBoxCliLoc->currentText().toInt());
        req.exec();

    //execution de la requete par le modele
    modelProdLoc.setQuery(req);

    //application du modele au tableView
    ui->tableViewProdLoc->setModel(&modelProdLoc);

    //nomination des colonnes
    modelProdLoc.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelProdLoc.setHeaderData(1, Qt::Horizontal, tr("Désignation"));
    modelProdLoc.setHeaderData(2, Qt::Horizontal, tr("Valeur"));

    //affichage du nom
    ui->comboBoxCliLoc->setModelColumn(1);

    //redimention
    ui->tableViewProdLoc->resizeColumnsToContents();
}

void BDM::on_comboBoxCat_activated(const QString &arg1)
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

    //récupération de l'id dans la varianble
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
    modelLocation->setHeaderData(0, Qt::Horizontal, tr("Désignation"));
    modelLocation->setHeaderData(1, Qt::Horizontal, tr("Quantité"));
    modelLocation->setHeaderData(2, Qt::Horizontal, tr("Date Départ"));
    modelLocation->setHeaderData(3, Qt::Horizontal, tr("Dat Arrivé"));
    ui->tableWidgetProduitLoc->resizeColumnsToContents();
    ui->tableWidgetProduitLoc->insertRow(ui->tableWidgetProduitLoc->rowCount());
    //Designation
    ui->comboBoxProd->setModelColumn(1);
    QTableWidgetItem * itemDesProd=new QTableWidgetItem(ui->comboBoxProd->currentText());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,0,itemDesProd);
    //Quantité
    QTableWidgetItem * itemQTTProd=new QTableWidgetItem(QString::number(ui->spinBoxQtt->value()));
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,1,itemQTTProd);
    //Taille
    QTableWidgetItem * itemTail=new QTableWidgetItem(ui->comboBoxTaille->currentText());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,2,itemTail);
    //date départ
    QTableWidgetItem * itemDD=new QTableWidgetItem(ui->dateEditDepart->text());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,3,itemDD);
    //date arrivé
    QTableWidgetItem * itemDA=new QTableWidgetItem(ui->dateEditArrive->text());
    ui->tableWidgetProduitLoc->setItem(ui->tableWidgetProduitLoc->rowCount()-1,4,itemDA);
}

void BDM::on_pushButtonActCli_clicked()
{
    ui->comboBoxCli->setModelColumn(0);
    QSqlQuery reqDesact;
    reqDesact.prepare("update Client set typCli=1 where numCli=:id");
        reqDesact.bindValue(":id", ui->comboBoxCli->currentText().toInt());
        reqDesact.exec();
    //application du modele à la tableView
    ui->tableViewCli->setModel(&modelClientRecap);
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));
    ui->comboBoxCli->setModelColumn(1);
}

void BDM::on_pushButtonDesactCli_clicked()
{
    ui->comboBoxCli->setModelColumn(0);
    //nouvelle version
    //preparation de la requete
    QSqlQuery reqDesact;
    reqDesact.prepare("update Client set typCli=2 where numCli=:id");
        reqDesact.bindValue(":id", ui->comboBoxCli->currentText().toInt());
        reqDesact.exec();

    ui->tableViewCli->setModel(&modelClientRecap);
    modelClientRecap.setHeaderData(0, Qt::Horizontal, tr("Numéro"));
    modelClientRecap.setHeaderData(1, Qt::Horizontal, tr("Société"));
    modelClientRecap.setHeaderData(2, Qt::Horizontal, tr("Téléphone fixe"));
    modelClientRecap.setHeaderData(3, Qt::Horizontal, tr("Portable"));
    modelClientRecap.setHeaderData(4, Qt::Horizontal, tr("Adresse mail"));
    ui->comboBoxCli->setModelColumn(1);
}

void BDM::on_pushButtonEnregistrer_clicked()
{
    //ne marche pas encore
    QStringList listeValues;
    QModelIndex index=ui->tableViewRecherche->selectionModel()->currentIndex();
    int noLigne=ui->tableViewRecherche->selectionModel()->currentIndex().row();
    index=index.sibling(noLigne,0);
    QVariant numeroClient=index.data();
    listeValues.append(numeroClient.toString());
    QString reqCommande="INSERT INTO Location(,dateCom,numCli) values ("+listeValues.join(",")+")";
}

void BDM::on_pushButtonRendu_clicked()
{

}
