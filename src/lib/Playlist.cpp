#include "Playlist.hpp"
#include <common/Utility.hpp>

#include <fstream>
#include <iostream>
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

std::string Playlist::getFilepath()const {
	return filepath;
}

size_t Playlist::size()const {
	return musics.size();
}

void Playlist::refresh(Musics musics) {
	if(!valid){
		return;
	}

	// "Refreshing playlist " + filepath;
	// filtering
	for(Musics::iterator i = musics.begin();i != musics.end(); i++) {
		MusicFile * music = i->second;
		bool cont = true;
		if (std::find(artists.begin(), artists.end(), music->getArtist()) == artists.end()) {
			//// music->getArtist() + " : is not in artists list : " + implode(artists);
			cont = false;
		}

		const std::vector<std::string> splittedKeywords = music->getSplittedKeywords();
		if(cont) {
			cont = std::find_first_of(splittedKeywords.begin(), splittedKeywords.end(), without.begin(), without.end()) == splittedKeywords.end();
		}

		if(with.size() && cont) {
			cont = std::find_first_of(splittedKeywords.begin(), splittedKeywords.end(), with.begin(), with.end()) != splittedKeywords.end();
		}

		if(music->getRating() < rating && cont) {
			//// music->getFilepath() + " : rating does not match : " + Common::toString(music->getRating()) + " < " + Common::toString(vrating);
			cont = false;
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
			cont = false;
			/*
			// music->getFilepath() +
			   " is not in duration sequence : min = " + Common::toString(min) +
			   " and max = " + Common::toString(max) +
			   " and current = " + Common::toString(currentDuration);
			*/
		}

		if(cont) {
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
		return;
	}
	valid = false;

	std::string line;
	while(line != ENDHEADER) {
		std::getline(playlist, line);
		// line;
		if(!line.compare(0, ARTISTS.length(), ARTISTS)) {
			Common::split(line.substr(ARTISTS.size()), ",", artists);
			//cout << "Getting artists : " + Common::implode(artists) + "\n";
		} else if(!line.compare(0, GENRES.length(), GENRES)) {
			Common::split(line.substr(GENRES.size()), ",", genres);
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
			//cout << "Getting without keywords : " + Common::implode(without) + "\n";
		} else if(!line.compare(0, WITH.length(), WITH)) {
			Common::split(line.substr(WITH.size()), ",", with);
			//cout << "Getting with keywords : " + Common::implode(with) + "\n";
		} else {
			//cout << "Unrecognized setting : " + line;
		}
	}
}

void Playlist::save() {
	if(!valid){
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
		cout << "Updating playlist : " << filepath << '\n';
		ofstream m3u_file(m3u.c_str(), ios::out | ios::trunc);
		m3u_file << m3u_content.str();
		m3u_file.close();
		filecontent = m3u_content.str();
	} else {
		cout << "Playlist did not changed : " << filepath << '\n';
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
