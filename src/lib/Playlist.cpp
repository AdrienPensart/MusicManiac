#include "Playlist.hpp"
#include <common/Utility.hpp>

#include <fstream>
#include <iostream>
#include <set>
using namespace std;

const std::string HEADER = "#EXTM3U";
const std::string MUSICMANIAC = "#EXTREM:musicmaniac";
const std::string TYPE = "#EXTREM:type:";
const std::string ENDHEADER = "#EXTREM:endheader";
const std::string ARTISTS = "#EXTREM:artists:";
const std::string GENRES = "#EXTREM:genres:";
const std::string RATING = "#EXTREM:rating:";
const std::string MIN_DURATION = "#EXTREM:minDuration:";
const std::string MAX_DURATION = "#EXTREM:maxDuration:";
const std::string WITHOUT = "#EXTREM:without:";
const std::string WITH = "#EXTREM:with:";
const std::string INF = "#EXTINF: ";
const std::string UUID = "#EXTREM:uuid ";

Playlist::Playlist(const std::string& _filepath) :
    filepath(_filepath), rating(0), type(NOT_MUSICMANIAC), minDuration("00:00"), maxDuration("100:00") {
}

const std::string& Playlist::getFilepath()const {
	return filepath;
}

std::string Playlist::getFilename() const {
    return (filepath.size() ? filepath.substr(filepath.find_last_of("/\\") + 1) : filepath);
}

size_t Playlist::size()const {
	return musics.size();
}

MusicVector& Playlist::getMusics(){
    return musics;
}

const std::string& Playlist::getType(){
    return type;
}

void Playlist::setType(const std::string& _type){
    type = _type;
}

bool Playlist::isManual(){
    return type == MANUAL || type == MANUAL_AUTOGEN;
}

bool Playlist::isAutogen(){
    return type == AUTOGEN || type == MANUAL_AUTOGEN;
}

void Playlist::setFilepath(const std::string& _filepath){
    filepath = _filepath;
}

bool Playlist::conform(MusicFile *music){
    if (artists.size() && std::find(artists.begin(), artists.end(), music->getArtist()) == artists.end()) {
        //cout << "Artist criteria does not match\n";
        return false;
    }

    const std::vector<std::string> splittedKeywords = music->getSplittedKeywords();
    if(std::find_first_of(splittedKeywords.begin(), splittedKeywords.end(), without.begin(), without.end()) != splittedKeywords.end()) {
        //cout << "Without criteria does not match\n";
        return false;
    }

    if(with.size() && std::find_first_of(splittedKeywords.begin(), splittedKeywords.end(), with.begin(), with.end()) == splittedKeywords.end()) {
        //cout << "With criteria does not match\n";
        return false;
    }

    if(music->getRating() < rating) {
        //cout << "Rating criteria does not match " << music->getRating() << " < " << rating << "\n";
        return false;
    }

    unsigned int currentDuration = music->getDurationInSeconds();
    std::string tempMinDuration = minDuration;
    tempMinDuration.erase(std::remove(tempMinDuration.begin(), tempMinDuration.end(), ':'), tempMinDuration.end());
    unsigned int min = 0;
    Common::fromString(tempMinDuration, min);

    std::string tempMaxDuration = maxDuration;
    tempMaxDuration.erase(std::remove(tempMaxDuration.begin(), tempMaxDuration.end(), ':'), tempMaxDuration.end());
    unsigned int max = 0;
    Common::fromString(tempMaxDuration, max);

    if(min > currentDuration || max < currentDuration) {
        //cout << "Duration criteria does not match\n";
        return false;
    }
    return true;
}

void Playlist::refreshWith(const Musics& musicsCollection) {
    if(type == NOT_MUSICMANIAC){
		return;
	}

    musics.clear();
    if(type == MANUAL){
        cout << "continue loading of file " << filepath << endl;
        std::string line;
        while(std::getline(stream, line)) {
            if(line.compare(0, 1, "#")){
                if(musicsCollection.count(line)){
                    add(musicsCollection.at(line));
                }
            }
        }
    } else if(isAutogen()){
        for(const auto& p : musicsCollection) {
            MusicFile * music = p.second;
            if(conform(music)) {
                add(music);
            }
        }
    }
}

void Playlist::load() {
    if(!filepath.size()){
        return;
    }

	Common::read_file (filepath, filecontent);
    stream.str(filecontent);

	std::string header;
	std::string musicmaniac;
    std::getline(stream, header);
    std::getline(stream, musicmaniac);
	if(header != HEADER || musicmaniac != MUSICMANIAC) {
        type = NOT_MUSICMANIAC;
		return;
	}
	std::string line;
	while(line != ENDHEADER) {
        std::getline(stream, line);
        if(!line.compare(0, TYPE.length(), TYPE)){
            type = line.substr(TYPE.size());
        } else if(!line.compare(0, ARTISTS.length(), ARTISTS)) {
            Common::split(line.substr(ARTISTS.size()), ",", artists);
		} else if(!line.compare(0, GENRES.length(), GENRES)) {
            Common::split(line.substr(GENRES.size()), ",", genres);
		} else if(!line.compare(0, RATING.length(), RATING)) {
			Common::fromString(line.substr(RATING.size()), rating);
		} else if(!line.compare(0, MIN_DURATION.length(), MIN_DURATION)) {
			minDuration = line.substr(MIN_DURATION.size());
		} else if(!line.compare(0, MAX_DURATION.length(), MAX_DURATION)) {
			maxDuration = line.substr(MAX_DURATION.size());
		} else if(!line.compare(0, WITHOUT.length(), WITHOUT)) {
            Common::split(line.substr(WITHOUT.size()), ",", without);
		} else if(!line.compare(0, WITH.length(), WITH)) {
			Common::split(line.substr(WITH.size()), ",", with);
        }
	}
}

void Playlist::save() {
    if(type == NOT_MUSICMANIAC || !filepath.size()){
        cout << "Invalid playlist " << filepath << " not saving.\n";
		return;
	}

	const std::string m3uExt = ".m3u";
	if(filepath.size() >= m3uExt.size() && filepath.substr(filepath.size()-m3uExt.size()) != m3uExt) {
		filepath += m3uExt;
	}
	string m3u = filepath;

	ostringstream m3u_content;
	m3u_content << HEADER << '\n'
                << MUSICMANIAC << '\n'
                << TYPE << type << '\n'
                << ARTISTS << Common::implode(artists) << '\n'
				<< GENRES << Common::implode(genres) << '\n'
				<< RATING << rating << '\n'
				<< MIN_DURATION << minDuration << '\n'
				<< MAX_DURATION << maxDuration << '\n'
				<< WITHOUT << Common::implode(without) << '\n'
				<< WITH << Common::implode(with) << '\n'
				<< ENDHEADER << '\n';

    for(const auto& m : musics) {
		size_t found = filepath.find_last_of("/");
        m3u_content << INF << m->getDurationInSeconds() << "," << m->getFilepath().substr(found+1) << '\n'
                    << UUID << m->getUUID() << '\n'
                    << m->getFilepath() << '\n';
	}

	if(filecontent != m3u_content.str()){
        //cout << "Updating playlist : " << filepath << endl;
		ofstream m3u_file(m3u.c_str(), ios::out | ios::trunc);
		m3u_file << m3u_content.str();
		m3u_file.close();
		filecontent = m3u_content.str();
        //cout << "Playlist saved : " << filecontent << endl;
	} else {
        //cout << "Playlist did not changed : " << filepath << endl;
	}
}

void Playlist::add(MusicFile * music) {
    musics.push_back(music);
}

double Playlist::getRating() {
	return rating;
}

void Playlist::setRating(double _rating) {
	rating = _rating;
}

std::string Playlist::getDuration(){
    int totalSeconds = 0;
    for(const auto& song : musics){
        totalSeconds += song->getDurationInSeconds();
    }
    return secondsToString(totalSeconds);
}

double Playlist::getAverageRating() {
    if(!size()){
        return 0.0;
    }

    double totalRating = 0.0;
    for(const auto& song : musics){
        totalRating += song->getRating();
    }
    return totalRating /= (double)size();
}

std::string Playlist::getMaxDuration() {
	return maxDuration;
}

void Playlist::setMaxDuration(const std::string& _maxDuration) {
	maxDuration = _maxDuration;
}

const std::set<std::string>& Playlist::getArtists() {
	return artists;
}

void Playlist::setArtists(const std::set<std::string>& _artists) {
	artists = _artists;
}

std::string Playlist::getMinDuration() {
	return minDuration;
}

void Playlist::setMinDuration(const std::string& _minDuration) {
	minDuration = _minDuration;
}

const std::set<std::string>& Playlist::getWith() {
	return with;
}

void Playlist::setWith(const std::set<std::string>& _with) {
	with = _with;
}

const std::set<std::string>& Playlist::getWithout() {
	return without;
}

void Playlist::setWithout(const std::set<std::string>& _without) {
	without = _without;
}

const std::set<std::string>& Playlist::getGenres() {
	return genres;
}

void Playlist::setGenres(const std::set<std::string>& _genres) {
	genres = _genres;
}
