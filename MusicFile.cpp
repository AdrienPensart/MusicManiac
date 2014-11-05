#include "MusicFile.hpp"
#include <cmath>
#include <QDebug>

MusicFile::MusicFile(QString _filepath, TagLib::File * _file)
    :filepath(_filepath), file(_file), modified(false){
    TagLib::AudioProperties * properties = file->audioProperties();
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
            duration = QString().sprintf("%02d:%02d", minutes, seconds);
        } else {
            duration = QString().sprintf("%d:%02d:%02d", hours, minutes, seconds);
        }
    } else {
        qDebug() << "No duration properties for " << filepath;
    }
}

MusicFile::~MusicFile(){
    save();
    delete file;
}

void MusicFile::save(){
    if(modified){
        qDebug() << "Saving file " << filepath;
        file->save();
    }
}

bool MusicFile::isModified(){
    return modified;
}

QString MusicFile::getFilepath(){
    return filepath;
}

QString MusicFile::getDuration(){
    return duration;
}

void MusicFile::setRating(double _rating, bool erase){
    if(rating != _rating){
        modified = erase;
        rating = _rating;
        qDebug() << "Setting rating : " << rating;
    }
}

void MusicFile::setUUID(QString _uuid, bool erase){
    if(uuid != _uuid){
        modified = erase;
        uuid = _uuid;
        qDebug() << "Setting UUID " << uuid;
    }
}

void MusicFile::setKeywords(QString _keywords, bool erase){
    if(keywords != _keywords){
        modified = erase;
        keywords = _keywords;
        qDebug() << "Setting keywords : " << keywords;
    }
}

double MusicFile::getRating(){
    return rating;
}

QString MusicFile::getUUID(){
    return uuid;
}

QString MusicFile::getKeywords(){
    return keywords;
}
