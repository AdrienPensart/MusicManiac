#include <iostream>
using namespace std;

#include <QFileDialog>

#include "ui_mainwindow.h"

#include "MainWindow.hpp"
#include "MusicFileFactory.hpp"
#include "MusicFolderModel.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
    connect(ui->actionOpen_Folder, SIGNAL(triggered()), this, SLOT(loadFolder()));
    connect(ui->actionRegen_UUID, SIGNAL(triggered()), this, SLOT(loadFolderWithRegen()));
    connect(ui->inWithButton, SIGNAL(clicked()), this, SLOT(availableToWith()));
    connect(ui->outWithButton, SIGNAL(clicked()), this, SLOT(withToAvailable()));
    connect(ui->inWithoutButton, SIGNAL(clicked()), this, SLOT(availableToWithout()));
    connect(ui->outWithoutButton, SIGNAL(clicked()), this, SLOT(withoutToAvailable()));

    musicProxyModel = new CustomSortFilterProxyModel(withoutKeywordsModel, withKeywordsModel, this);
    connect(ui->ratingSpinBox, SIGNAL(valueChanged(double)), musicProxyModel, SLOT(ratingChanged(double)));
    connect(ui->minDurationEdit, SIGNAL(textChanged(QString)), musicProxyModel, SLOT(minDurationChanged(QString)));
    connect(ui->maxDurationEdit, SIGNAL(textChanged(QString)), musicProxyModel, SLOT(maxDurationChanged(QString)));

    withoutKeywordsSelection = new QItemSelectionModel(&withoutKeywordsModel);
    availableKeywordsSelection = new QItemSelectionModel(&availableKeywordsModel);
    withKeywordsSelection = new QItemSelectionModel(&withKeywordsModel);

    ui->withoutKeywordsView->setModel(&withoutKeywordsModel);
    ui->withoutKeywordsView->setSelectionModel(withoutKeywordsSelection);

    ui->availableKeywordsView->setModel(&availableKeywordsModel);
    ui->availableKeywordsView->setSelectionModel(availableKeywordsSelection);

    ui->withKeywordsView->setModel(&withKeywordsModel);
    ui->withKeywordsView->setSelectionModel(withKeywordsSelection);

    musicModel = new MusicFolderModel(this);
    musicProxyModel->setSourceModel(musicModel);
    ui->musicView->setModel(musicProxyModel);
    ui->musicView->setSortingEnabled(true);
    ui->musicView->horizontalHeader()->setStretchLastSection(true);
    musicProxyModel->setFilterKeyColumn(MusicFolderModel::COLUMN_KEYWORDS);
    musicProxyModel->setDynamicSortFilter(true);
}

MainWindow::~MainWindow(){
    delete ui;
    cout << "MainWindow destructor" << endl;
}

void MainWindow::selectionToModel(
        QItemSelectionModel * sourceSelection,
        QItemSelectionModel * /*destinationSelection*/,
        QStringListModel& sourceModel,
        QStringListModel& destinationModel){
    QModelIndexList indexes = sourceSelection->selection().indexes();
    QStringList list = destinationModel.stringList();
    while(indexes.size()) {
        list.append(sourceModel.data(indexes.first(), Qt::DisplayRole).toString());
        sourceModel.removeRow(indexes.first().row());
        indexes = sourceSelection->selection().indexes();
    }
    destinationModel.setStringList(list);
    musicProxyModel->refilter();
}

void MainWindow::withoutToAvailable(){
    selectionToModel(withoutKeywordsSelection, availableKeywordsSelection, withoutKeywordsModel, availableKeywordsModel);
}

void MainWindow::availableToWithout(){
    selectionToModel(availableKeywordsSelection, withoutKeywordsSelection, availableKeywordsModel, withoutKeywordsModel);
}

void MainWindow::availableToWith(){
    selectionToModel(availableKeywordsSelection, withKeywordsSelection, availableKeywordsModel, withKeywordsModel);
}

void MainWindow::withToAvailable(){
    selectionToModel(withKeywordsSelection, availableKeywordsSelection, withKeywordsModel, availableKeywordsModel);
}

void MainWindow::loadFolder(){
    loadFolderWith(false);
}

void MainWindow::loadFolderWithRegen(){
    loadFolderWith(true);
}

void MainWindow::loadFolderWith(bool regen){
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!folderPath.size())
    {
        cout << "Invalid folder" << endl;
        return;
    }

    musicModel->clear();
    MusicFileFactory mff(folderPath, regen);
    while(mff.valid()){
        MusicFile * mf = 0;
        mf = mff.factory();
        if(mf){
            musicModel->add(mf);
        }
    }

    availableKeywordsModel.setStringList(musicModel->getKeywords());
    ui->musicView->resizeColumnsToContents();
    ui->musicView->reset();
    musicProxyModel->refilter();
}
