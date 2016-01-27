#include "Playlist.hpp"
#include <common/Utility.hpp>

#include <fstream>
#include <iostream>
#include <set>
using namespace std;

const std::string HEADER = "#EXTM3U";
const std::string MUSICMANIAC = "#EXTREM:musicmaniac";
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
	filepath(_filepath), rating(0), valid(true), minDuration("00:00"), maxDuration("100:00") {
}

const std::string& Playlist::getFilepath()const {
	return filepath;
}

std::string Playlist::getFilename() const {
	return filepath.substr(filepath.find_last_of("/\\") + 1);
}

size_t Playlist::size()const {
	return musics.size();
}

const Musics& Playlist::getMusics(){
    return musics;
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

void Playlist::refreshWith(const Musics& musics) {
    if(!valid){
		return;
	}

	// "Refreshing playlist " + filepath;
	// filtering
    for(const auto& p : musics) {
        MusicFile * music = p.second;
        if(conform(music)) {
            // "ADDING : " + music->getFilepath();
            add(music);
        }
	}
}

void Playlist::load() {
	Common::read_file (filepath, filecontent);
	std::istringstream playlist(filecontent);

	std::string header;
	std::string musicmaniac;
	std::getline(playlist, header);
	std::getline(playlist, musicmaniac);
	if(header != HEADER || musicmaniac != MUSICMANIAC) {
		cout << "Not a MusicManiac playlist : " + filepath + "\n";
        valid = false;
		return;
	}

	std::string line;
	while(line != ENDHEADER) {
		std::getline(playlist, line);
		// line;
		if(!line.compare(0, ARTISTS.length(), ARTISTS)) {
			Common::split(line.substr(ARTISTS.size()), ",", artists);
            std::set<std::string> s( artists.begin(), artists.end() );
            artists.assign( s.begin(), s.end() );
			//cout << "Getting artists : " + Common::implode(artists) + "\n";
		} else if(!line.compare(0, GENRES.length(), GENRES)) {
			Common::split(line.substr(GENRES.size()), ",", genres);
            std::set<std::string> s( genres.begin(), genres.end() );
            genres.assign( s.begin(), s.end() );
			//cout << "Getting genres : " + Common::implode(genres) + "\n";
		} else if(!line.compare(0, RATING.length(), RATING)) {
			Common::fromString(line.substr(RATING.size()), rating);
			//cout << "Getting rating : " + Common::toString(rating) + "\n";
		} else if(!line.compare(0, MIN_DURATION.length(), MIN_DURATION)) {
			minDuration = line.substr(MIN_DURATION.size());
			//cout << "Getting min duration : " + minDuration + "\n";
		} else if(!line.compare(0, MAX_DURATION.length(), MAX_DURATION)) {
			maxDuration = line.substr(MAX_DURATION.size());
			//cout << "Getting max duration : " + maxDuration + "\n";
		} else if(!line.compare(0, WITHOUT.length(), WITHOUT)) {
			Common::split(line.substr(WITHOUT.size()), ",", without);
            std::set<std::string> s( without.begin(), without.end() );
            without.assign( s.begin(), s.end() );
			//cout << "Getting without keywords : " + Common::implode(without) + "\n";
		} else if(!line.compare(0, WITH.length(), WITH)) {
			Common::split(line.substr(WITH.size()), ",", with);
            std::set<std::string> s( with.begin(), with.end() );
            with.assign( s.begin(), s.end() );
			//cout << "Getting with keywords : " + Common::implode(with) + "\n";
		} else {
			//cout << "Unrecognized setting : " + line;
		}
	}

    cout << "Loading of playlist " << filepath << " ok\n";
}

void Playlist::save() {
	if(!valid){
        cout << "Invalid playlist " << filepath << " not saving\n";
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
				<< ARTISTS << Common::implode(artists) << '\n'
				<< GENRES << Common::implode(genres) << '\n'
				<< RATING << rating << '\n'
				<< MIN_DURATION << minDuration << '\n'
				<< MAX_DURATION << maxDuration << '\n'
				<< WITHOUT << Common::implode(without) << '\n'
				<< WITH << Common::implode(with) << '\n'
				<< ENDHEADER << '\n';

	for(Musics::const_iterator i = musics.begin(); i != musics.end(); i++) {
		size_t found = filepath.find_last_of("/");
		m3u_content << INF << i->second->getDurationInSeconds() << "," << i->first.substr(found+1) << '\n'
					<< UUID << i->second->getUUID() << '\n'
					<< i->first << '\n';
	}

	if(filecontent != m3u_content.str()){
		cout << "Updating playlist : " << filepath << endl;
		ofstream m3u_file(m3u.c_str(), ios::out | ios::trunc);
		m3u_file << m3u_content.str();
		m3u_file.close();
		filecontent = m3u_content.str();
        cout << "Playlist saved : " << filecontent << endl;
	} else {
		cout << "Playlist did not changed : " << filepath << endl;
	}
}

void Playlist::add(MusicFile * music) {
	musics[music->getFilepath()] = music;
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
        totalSeconds += song.second->getDurationInSeconds();
    }
    return secondsToString(totalSeconds);
}

double Playlist::getAverageRating() {
    double totalRating = 0.0;
    for(const auto& song : musics){
        totalRating += song.second->getRating();
    }
    return totalRating /= (double)size();
}

std::string Playlist::getMaxDuration() {
	return maxDuration;
}

void Playlist::setMaxDuration(const std::string& _maxDuration) {
	maxDuration = _maxDuration;
}

const std::vector<std::string>& Playlist::getArtists() {
	return artists;
}

void Playlist::setArtists(const std::vector<std::string>& _artists) {
	artists = _artists;
}

std::string Playlist::getMinDuration() {
	return minDuration;
}

void Playlist::setMinDuration(const std::string& _minDuration) {
	minDuration = _minDuration;
}

const std::vector<std::string>& Playlist::getWith() {
	return with;
}

void Playlist::setWith(const std::vector<std::string>& _with) {
	with = _with;
}

const std::vector<std::string>& Playlist::getWithout() {
	return without;
}

void Playlist::setWithout(const std::vector<std::string>& _without) {
	without = _without;
}

const std::vector<std::string>& Playlist::getGenres() {
	return genres;
}

void Playlist::setGenres(const std::vector<std::string>& _genres) {
	genres = _genres;
}
