#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <iostream>
#include <iomanip>
using namespace std;

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>
#include <taglib/uniquefileidentifierframe.h>
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/xiphcomment.h>
using namespace TagLib;

#include <QUuid>
#include <QDebug>
#include <QFileDialog>
#include <QDirIterator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
    connect(ui->actionOpen_Folder, SIGNAL(triggered()), this, SLOT(loadFolder()));
    connect(ui->actionAdd_missing_MP3_UUID, SIGNAL(triggered()), this, SLOT(addMissingMP3UUID()));
    connect(ui->actionAdd_missing_FLAC_UUID, SIGNAL(triggered()), this, SLOT(addMissingFLACUUID()));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::loadFolder(){
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home/crunch/music", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!folderPath.size())
    {
        return;
    }

    QDir music(folderPath);
    scanDirIter(music);
}

void MainWindow::addMissingFLACUUID(){
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),"/home/crunch/music/test",tr("Music (*.flac)"));
    if(!filePath.size()){
        cout << "No file path given" << endl;
        return;
    }

    FLAC::File f(filePath.toStdString().c_str());
    if(!f.hasXiphComment()){
        cout << "No XiphComment present" << endl;
        return;
    }

    Ogg::XiphComment * tag = f.xiphComment();
    if(!tag){
        cout << "Tag invalid" << endl;
        return;
    }

    if(!tag->contains("UFID")){
        cout << "No UFID frame" << endl;
        QUuid quuid = QUuid::createUuid();
        string uuid = quuid.toString().toStdString();
        cout << "Generating UUID : " << uuid << endl;
        tag->addField("UFID", uuid.c_str());
        f.save();
    } else {
        StringList list = tag->properties()["UFID"];
        if(list.size()){
            cout << "UFID BEGIN" << endl;
            cout << list.front() << endl;
            cout << "UFID BEGIN" << endl;
        }
    }
}

void MainWindow::addMissingMP3UUID(){
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),"/home/crunch/music/test",tr("Music (*.mp3)"));
    if(!filePath.size()){
        cout << "No file path given" << endl;
        return;
    }

    MPEG::File f(filePath.toStdString().c_str());
    if(!f.hasID3v2Tag()){
        cout << "No ID3v2 Tag present" << endl;
        return;
    }

    ID3v2::Tag * tag = f.ID3v2Tag();
    if(!tag){
        cout << "Tag invalid" << endl;
        return;
    }

    const ID3v2::FrameListMap& frames = tag->frameListMap();
    ID3v2::FrameListMap::ConstIterator ufidIter = frames.find("UFID");
    if(ufidIter == frames.end()){
        cout << "No UFID frame" << endl;
        QUuid quuid = QUuid::createUuid();
        string uuid = quuid.toString().toStdString();
        cout << "Generating UUID : " << uuid << endl;
        ID3v2::UniqueFileIdentifierFrame * ufid = new ID3v2::UniqueFileIdentifierFrame("braincraft", uuid.c_str());
        tag->addFrame(ufid);
        f.save();
        return;
    } else {
        ID3v2::UniqueFileIdentifierFrame * ufid = (ID3v2::UniqueFileIdentifierFrame *)ufidIter->second.front();
        cout << "UFID BEGIN" << endl;
        cout << ufid->owner() << " " << ufid->identifier() << endl;
        cout << "UFID BEGIN" << endl;
    }
}

void MainWindow::scanDirIter(QDir dir){
    ui->tableWidget->setRowCount(0);
    QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (iterator.fileInfo().isDir()){
            continue;
        }

        TagLib::FileRef f(iterator.filePath().toStdString().c_str());
        if(f.isNull() || !f.audioProperties()){
            continue;
        }

        TagLib::AudioProperties *properties = f.audioProperties();
        int totalSeconds;
        int seconds;
        int minutes;
        int hours;

        totalSeconds = properties->length();
        hours = floor(totalSeconds / 3600);
        minutes = floor((totalSeconds - hours * 3600) / 60);
        seconds = floor(totalSeconds - hours * 3600 - minutes * 60);

        QString totalDurationText;
        if(hours <= 0) {
            totalDurationText = QString().sprintf("%02d:%02d", minutes, seconds);
        } else {
            totalDurationText = QString().sprintf("%d:%02d:%02d", hours, minutes, seconds);
        }

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        QTableWidgetItem* path = new QTableWidgetItem();
        path->setText(iterator.filePath());
        ui->tableWidget->setItem(row, 0, path);

        QTableWidgetItem* duration = new QTableWidgetItem();
        duration->setText(totalDurationText);
        ui->tableWidget->setItem(row, 1, duration);

        TagLib::PropertyMap tags = f.file()->properties();
        TagLib::StringList list = tags["FMPS_RATING"];
        if(list.size() == 1){
            QTableWidgetItem* ratingItem = new QTableWidgetItem();
            QString ratingStr = QString::fromStdString(list[0].to8Bit(true));
            double rating = ratingStr.toDouble();
            rating *= 5;
            ratingStr = QString::number(rating);
            ratingItem->setText(ratingStr);
            ui->tableWidget->setItem(row, 2, ratingItem);
        }

        //QTableWidgetItem* uuid = new QTableWidgetItem();
        //filesTable->setItem(row, 1, sizeItem);
    }
}
