#include "MusicFile.hpp"
#include "common/Utility.hpp"
#include <cmath>
#include <cstdio>
#include <string>

MusicFile::MusicFile(std::string _filepath, TagLib::File * _file)
    :totalSeconds(-1), filepath(_filepath), file(_file), modified(false), rating(0){
    TagLib::AudioProperties * properties = file->audioProperties();
    if(properties){
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
        LOG << filepath + ", duration : " + duration;
    } else {
        LOG << filepath + ", no duration";
    }
}

MusicFile::~MusicFile(){
    save();
    delete file;
}

void MusicFile::save(){
    if(modified){
        LOG << "saving " + filepath;
        if(!file->save()){
            LOG << "Failed.";
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

std::string MusicFile::getFilepath() const {
    return filepath;
}

std::string MusicFile::getDuration() const {
    return duration;
}

unsigned int MusicFile::getDurationInSeconds() const {
    return totalSeconds;
}

void MusicFile::setRating(double _rating, bool erase){
    if(_rating < 0 || _rating > 5){
        _rating = 0;
    }

    if(rating != _rating){
        modified |= erase;
        rating = _rating;
        LOG << filepath + ", setting rating : " + Common::toString(rating) + " modified ? : " + Common::toString(modified);
    }
}

void MusicFile::setUUID(std::string _uuid, bool erase){
    if(uuid != _uuid){
        modified |= erase;
        uuid = _uuid;
        LOG << filepath + ", setting UUID " + uuid + " modified ? : " + Common::toString(modified);
    }
}

void MusicFile::setKeywords(std::string _keywords, bool erase){
    if(keywords != _keywords){
        modified |= erase;
        keywords = _keywords;
        LOG << filepath + ", setting keywords : " + keywords + " modified ? : " + Common::toString(modified);
    }
}

double MusicFile::getRating() const {
    return rating;
}

std::string MusicFile::getUUID() const {
    return uuid;
}

std::string MusicFile::getKeywords() const {
    return keywords;
}

std::vector<std::string> MusicFile::getSplittedKeywords() const {
    std::vector<std::string> results;
    Common::split(keywords, " ", results);
    return results;
}
