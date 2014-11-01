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
    connect(ui->actionOpen_Folder, SIGNAL(triggered()), this, SLOT(chooseFolder()));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::chooseFolder(){
    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home/crunch/music", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!folderPath.size())
    {
        qDebug() << "Invalid folder";
        return;
    }

    QDir music(folderPath);
    qDebug() << "Scanning folder " << folderPath;
    scanFolder(music);
}

void MainWindow::scanFolder(QDir dir){
    ui->tableWidget->setRowCount(0);
    QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
    while(iterator.hasNext()){
        iterator.next();
        if (!iterator.fileInfo().isDir()){
            if(iterator.filePath().endsWith(".mp3")){
                qDebug() << "Adding mp3 " << iterator.filePath();
                addMP3(iterator.filePath());
            }
            else if(iterator.filePath().endsWith(".flac")){
                qDebug() << "Adding flac " << iterator.filePath();
                addFLAC(iterator.filePath());
            }
            else {
                qDebug() << "Music file not supported " << iterator.filePath();
            }
        }
    }
}

void MainWindow::addMP3(QString filepath){
    MPEG::File f(filepath.toStdString().c_str());
    if(!f.audioProperties()){
        qDebug() << "No audio property";
        return;
    }

    if(!f.hasID3v2Tag()){
        qDebug() << "No ID3v2 Tag present";
        return;
    }

    ID3v2::Tag * tag = f.ID3v2Tag();
    if(!tag){
        qDebug() << "Tag invalid";
        return;
    }

    const ID3v2::FrameListMap& frames = tag->frameListMap();
    ID3v2::FrameListMap::ConstIterator ufidIter = frames.find("UFID");
    QString uuid;
    if(ufidIter == frames.end()){
        qDebug() << "No UFID frame";
        QUuid quuid = QUuid::createUuid();
        uuid = quuid.toString();
        qDebug() << "Generating UUID : " << uuid;
        ID3v2::UniqueFileIdentifierFrame * ufid = new ID3v2::UniqueFileIdentifierFrame("braincraft", uuid.toStdString().c_str());
        tag->addFrame(ufid);
        //f.save();
    } else {
        ID3v2::UniqueFileIdentifierFrame * ufid = (ID3v2::UniqueFileIdentifierFrame *)ufidIter->second.front();
        uuid = ufid->identifier().data();
    }

    TagLib::PropertyMap tags = f.properties();
    TagLib::StringList list = tags["FMPS_RATING"];
    double rating = 0;
    if(list.size() == 1){
        QString ratingStr = QString::fromStdString(list[0].to8Bit(true));
        rating = ratingStr.toDouble();
        rating *= 5;
    }

    addRow(filepath, getDuration(&f), rating, uuid);
}

void MainWindow::addFLAC(QString filepath){
    FLAC::File f(filepath.toStdString().c_str());
    if(!f.audioProperties()){
        qDebug() << "No audio property";
        return;
    }

    if(!f.hasXiphComment()){
        qDebug() << "No XiphComment present";
        return;
    }

    Ogg::XiphComment * tag = f.xiphComment();
    if(!tag){
        qDebug() << "Tag invalid";
        return;
    }

    QString uuid;
    if(!tag->contains("UFID")){
        qDebug() << "No UFID frame";
        QUuid quuid = QUuid::createUuid();
        uuid = quuid.toString();
        tag->addField("UFID", uuid.toStdString().c_str());
        //f.save();
    } else {
        StringList list = tag->properties()["UFID"];
        if(list.size()){
            uuid = list.front().toCString();
        }
    }

    double rating = 0;
    if(!tag->contains("FMPS_RATING")){
        qDebug() << "No rating";
    } else {
        StringList list = tag->properties()["FMPS_RATING"];
        if(list.size()){
            QString ratingStr = QString::fromStdString(list[0].to8Bit(true));
            rating = ratingStr.toDouble();
            rating *= 5;
        }
    }

    addRow(filepath, getDuration(&f), rating, uuid);
}

QString MainWindow::getDuration(TagLib::File * file){
    TagLib::AudioProperties * properties = file->audioProperties();
    QString totalDurationText;
    if(properties){
        int totalSeconds;
        int seconds;
        int minutes;
        int hours;

        totalSeconds = properties->length();
        hours = floor(totalSeconds / 3600);
        minutes = floor((totalSeconds - hours * 3600) / 60);
        seconds = floor(totalSeconds - hours * 3600 - minutes * 60);


        if(hours <= 0) {
            totalDurationText = QString().sprintf("%02d:%02d", minutes, seconds);
        } else {
            totalDurationText = QString().sprintf("%d:%02d:%02d", hours, minutes, seconds);
        }
    }
    return totalDurationText;
}

void MainWindow::addRow(QString filepath, QString duration, double rating, QString uuid){
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    QTableWidgetItem* pathItem = new QTableWidgetItem();
    pathItem->setText(filepath);
    ui->tableWidget->setItem(row, 0, pathItem);

    QTableWidgetItem* durationItem = new QTableWidgetItem();
    durationItem->setText(duration);
    ui->tableWidget->setItem(row, 1, durationItem);

    QTableWidgetItem* ratingItem = new QTableWidgetItem();
    QString ratingStr = QString::number(rating);
    ratingItem->setText(ratingStr);
    ui->tableWidget->setItem(row, 2, ratingItem);

    QTableWidgetItem* uuidItem = new QTableWidgetItem();
    uuidItem->setText(uuid);
    ui->tableWidget->setItem(row, 3, uuidItem);
}
