#include "MusicFile.hpp"
#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;

MusicFile::MusicFile(string _filepath, TagLib::File * _file)
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

        char buffer[256];
        if(hours <= 0) {
             snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        } else {
             snprintf(buffer, sizeof(buffer), "%d:%02d:%02d", hours, minutes, seconds);
        }
        duration = buffer;
    } else {
        cout << "No duration properties for " << filepath << endl;
    }
}

MusicFile::~MusicFile(){
    save();
    delete file;
}

void MusicFile::save(){
    if(modified){
        cout << "Saving file " << filepath << endl;
        if(!file->save()){
            cout << "Failed.";
        } else {
            modified = false;
        }
    }
}

void MusicFile::touch(){
    modified = true;
}

bool MusicFile::isModified(){
    return modified;
}

string MusicFile::getFilepath() const {
    return filepath;
}

string MusicFile::getDuration() const {
    return duration;
}

void MusicFile::setRating(double _rating, bool erase){
    if(rating != _rating){
        modified |= erase;
        rating = _rating;
        cout << "Setting rating : " << rating << " modified ? : " << modified << endl;
    }
}

void MusicFile::setUUID(string _uuid, bool erase){
    if(uuid != _uuid){
        modified |= erase;
        uuid = _uuid;
        cout << "Setting UUID " << uuid << " modified ? : " << modified << endl;
    }
}

void MusicFile::setKeywords(string _keywords, bool erase){
    if(keywords != _keywords){
        modified |= erase;
        keywords = _keywords;
        cout << "Setting keywords : " << keywords << " modified ? : " << modified << endl;
    }
}

double MusicFile::getRating() const {
    return rating;
}

string MusicFile::getUUID() const {
    return uuid;
}

string MusicFile::getKeywords() const {
    return keywords;
}
