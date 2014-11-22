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

    musicModel = new MusicFolderModel(this);
    ui->musicView->setModel(musicModel);
    //ui->musicView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch );
    ui->musicView->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow(){
    delete ui;
    cout << "MainWindow destructor" << endl;
}

void MainWindow::loadFolder(){
    loadFolderWith(false);
}

void MainWindow::loadFolderWithRegen(){
    loadFolderWith(true);
}

void MainWindow::loadFolderWith(bool regen){
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home/crunch/music", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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
    ui->musicView->reset();
}
