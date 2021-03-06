#include "MusicFile.hpp"
#include "common/Utility.hpp"
#include <cmath>
#include <cstdio>
#include <string>

std::string secondsToString(int totalSeconds){
    int hours = floor(totalSeconds / 3600);
    int minutes = floor((totalSeconds - hours * 3600) / 60);
    int seconds = floor(totalSeconds - hours * 3600 - minutes * 60);

    char buffer[256];
    if(hours <= 0) {
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
    } else {
        snprintf(buffer, sizeof(buffer), "%d:%02d:%02d", hours, minutes, seconds);
    }
    return buffer;
}

MusicFile::MusicFile(const std::string& _filepath, TagLib::File * _file)
	:totalSeconds(-1), filepath(_filepath), file(_file), modified(false), rating(0) {
	TagLib::AudioProperties * properties = file->audioProperties();
	if(properties) {
        totalSeconds = properties->length();
        duration = secondsToString(totalSeconds);
		// filepath + ", duration : " + duration;
	} else {
		// filepath + ", no duration";
	}
}

MusicFile::~MusicFile() {
	save();
	delete file;
}

void MusicFile::save() {
	if(modified) {
		// "saving " + filepath;
		if(!file->save()) {
			// "Failed.";
		} else {
			modified = false;
		}
	}
}

void MusicFile::touch() {
	modified = true;
}

bool MusicFile::isModified() {
	return modified;
}

std::string MusicFile::getPath() const {
	return filepath.substr(0,filepath.find_last_of("/\\"));
}

const std::string& MusicFile::getFilepath() const {
	return filepath;
}

std::string MusicFile::getFilename() const {
	return filepath.substr(filepath.find_last_of("/\\") + 1);
}

std::string MusicFile::getDuration() const {
	return duration;
}

std::string MusicFile::getYoutube() const {
	return youtube;
}

std::string MusicFile::getTitle() const {
	return title;
}

int MusicFile::getDurationInSeconds() const {
	return totalSeconds;
}

void MusicFile::setYoutube(std::string _youtube){
	youtube = _youtube;
}

void MusicFile::setRating(double _rating, bool erase) {
	if(_rating < 0 || _rating > 5) {
		_rating = 0;
	}

	if(rating != _rating) {
		modified |= erase;
		rating = _rating;
		// filepath + ", setting rating : " + Common::toString(rating) + " modified ? : " + Common::toString(modified);
	}
}

void MusicFile::setUUID(std::string _uuid, bool erase) {
	if(uuid != _uuid) {
		modified |= erase;
		uuid = _uuid;
		// filepath + ", setting UUID " + uuid + " modified ? : " + Common::toString(modified);
	}
}

void MusicFile::setArtist(std::string _artist, bool erase) {
	if(artist != _artist) {
		modified |= erase;
		artist = _artist;
		// filepath + ", setting artist " + artist + " modified ? : " + Common::toString(modified);
	}
}

void MusicFile::setTitle(std::string _title, bool erase){
	if(title != _title) {
		modified |= erase;
		title = _title;
		// filepath + ", setting title " + title + " modified ? : " + Common::toString(title);
	}
}

void MusicFile::setGenre(std::string _genre, bool erase) {
	if(genre != _genre) {
		modified |= erase;
		genre = _genre;
		// filepath + ", setting genre " + genre + " modified ? : " + Common::toString(modified);
	}
}

void MusicFile::setKeywords(std::string _keywords, bool erase) {
	if(keywords != _keywords) {
		modified |= erase;
		keywords = _keywords;
		// filepath + ", setting keywords : " + keywords + " modified ? : " + Common::toString(modified);
	}
}

void MusicFile::setAlbum(std::string _album, bool erase){
	if(album != _album) {
		modified |= erase;
		album = _album;
		// filepath + ", setting album : " + album + " modified ? : " + Common::toString(modified);
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

std::string MusicFile::getArtist() const {
	return artist;
}

std::string MusicFile::getGenre() const {
	return genre;
}

std::string MusicFile::getAlbum() const {
	return album;
}

std::vector<std::string> MusicFile::getSplittedKeywords() const {
	std::vector<std::string> results;
	Common::split(keywords, " ", results);
	return results;
}
