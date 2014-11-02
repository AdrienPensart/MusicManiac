#include "MusicFile.hpp"
#include <cmath>
#include <QDebug>

MusicFile::MusicFile(QString _filepath, TagLib::File * _file)
    :filepath(_filepath), file(_file){
}

MusicFile::~MusicFile(){
    delete file;
}

QString MusicFile::getFilepath(){
    return filepath;
}

QString MusicFile::getDuration(){
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
        qDebug() << "Duration for " << filepath << " is " << totalDurationText;
    } else {
        qDebug() << "No duration properties for " << filepath;
    }
    return totalDurationText;
}
