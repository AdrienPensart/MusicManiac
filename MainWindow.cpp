#include <QDebug>
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

    musicModel = new MusicFolderModel();
    ui->musicView->setModel(musicModel);
    ui->musicView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::loadFolder(){
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home/crunch/music", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!folderPath.size())
    {
        qDebug() << "Invalid folder";
        return;
    }

    musicModel->clear();
    MusicFileFactory mff(folderPath);
    while(mff.valid()){
        MusicFile * mf = 0;
        mf = mff.factory();
        if(mf){
            musicModel->add(mf);
        }
    }

    ui->musicView->reset();
}
