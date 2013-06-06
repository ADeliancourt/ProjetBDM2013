-- Auteur:		Alexis Deliancourt
-- Date Création:	lundi 3 juin 2013, 09:29:39 (UTC+0200)
-- Date Modification:	lundi 3 juin 2013, 09:29:39 (UTC+0200)
-- Moteur SQL:		MySql

-- Sélection de la Base de Données
USE BDM;

-- Suppression du schéma précédent
DROP TABLE IF EXISTS Location, Statut, Produit, Taille, Categorie, Client, Type; 

-- Création du nouveau schéma
        -- Création de la table Type
CREATE TABLE Type(
        numTyp          tinyint auto_increment,
        libTyp          varchar(10),
        PRIMARY KEY     (numTyp)
)engine=innodb;

	-- Création de la table Client:
CREATE TABLE Client(
	numCli 		smallint auto_increment,
	libCli 		varchar(50),
	telECli		varchar(14),
	telPCli		varchar(14),
	melCli 		varchar(60),
	typCli		tinyint default 1,
	PRIMARY KEY 	(numCli)
)engine=innodb;

	-- Création de la table Categorie:
CREATE TABLE Categorie(
	numCat	 	tinyint auto_increment,
	libCat		varchar(20),
	PRIMARY KEY 	(numCat)
)engine=innodb;

        -- Création de la table Taille
CREATE TABLE Taille(
        numTai          tinyint auto_increment,
        libTai          varchar(10),
        PRIMARY KEY     (numTai)
)engine=innodb;

        -- Création de la table Produit:
CREATE TABLE Produit(
        numProd         tinyint auto_increment,
        libProd         varchar(30),
        stkTota		smallint,
	stkReel		smallint,
        altProd	        tinyint,
        priProd         float,
        catProd         tinyint,
	taiProd		tinyint default 1,
        PRIMARY KEY     (numProd)
)engine=innodb;

        -- Création de la table Statut
CREATE TABLE Statut(                                    -- +---+-------------+ --
        numStat         tinyint auto_increment,         -- | 1 | En location | --
        libStat         varchar(30),                    -- +---+-------------+ --
        PRIMARY KEY     (numStat)                       -- | 2 |    Rendu    | --
)engine=innodb;                                         -- +---+-------------+ --

	-- Création de la table Location
CREATE TABLE Location(
	numCli		smallint,
	numProd		tinyint,
	qttLoc		smallint,
	dateDepart	date,
	dateArrPrevue	date,
	dateArrReel	date,
	numStat		tinyint,
	PRIMARY KEY 	(numCli,numProd)
)engine=innodb;

-- Ajout des contraintes d'intégrité référencielle
	-- Mise des caractères en encodage UTF-8 --
ALTER DATABASE BDM charset=utf8;

        -- Client --> Type --
ALTER TABLE Client
        ADD FOREIGN KEY (typCLi) references Type (numTyp);

	-- Produit --> Categorie --
ALTER TABLE Produit
        ADD FOREIGN KEY (catProd) references Categorie (numCat);

        -- Produit --> Taille --
ALTER TABLE Produit
        ADD FOREIGN KEY (taiProd) references Taille (numTai);

	-- Location --> Client --
ALTER TABLE Location
	ADD FOREIGN KEY (numCli) references Client (numCli);

	-- Location --> Produit --
ALTER TABLE Location
	ADD FOREIGN KEY (numProd) references Produit (numProd);

	-- Location --> Statut --
ALTER TABLE Location
	ADD FOREIGN KEY (numStat) references Statut (numStat);
