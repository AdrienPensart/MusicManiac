#include "Playlist.hpp"
#include <common/Utility.hpp>

#include <fstream>
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
	filepath(_filepath) {
}

std::string Playlist::getFilepath()const {
	return filepath;
}

void Playlist::load() {
	ifstream playlist(filepath.c_str());
	std::string header;
	std::string musicmaniac;
	std::getline(playlist, header);
	std::getline(playlist, musicmaniac);
	if(header != HEADER || musicmaniac != MUSICMANIAC) {
		// "Not a MusicManiac playlist";
		return;
	}

	std::string line;
	while(line != ENDHEADER) {
		std::getline(playlist, line);
		// line;
		if(!line.compare(0, ARTISTS.length(), ARTISTS)) {
			Common::split(line.substr(ARTISTS.size()), ",", artists);
			// "Getting artists : " + Common::implode(artists);
		} else if(!line.compare(0, GENRES.length(), GENRES)) {
			Common::split(line.substr(GENRES.size()), ",", genres);
			// "Getting genres : " + Common::implode(genres);
		} else if(!line.compare(0, RATING.length(), RATING)) {
			Common::fromString(line.substr(RATING.size()), rating);
			// "Getting rating : " + Common::toString(rating);
		} else if(!line.compare(0, MIN_DURATION.length(), MIN_DURATION)) {
			minDuration = line.substr(MIN_DURATION.size());
			// "Getting min duration : " + minDuration;
		} else if(!line.compare(0, MAX_DURATION.length(), MAX_DURATION)) {
			maxDuration = line.substr(MAX_DURATION.size());
			// "Getting max duration : " + maxDuration;
		} else if(!line.compare(0, WITHOUT.length(), WITHOUT)) {
			Common::split(line.substr(WITHOUT.size()), ",", without);
			// "Getting without keywords : " + Common::implode(without);
		} else if(!line.compare(0, WITH.length(), WITH)) {
			Common::split(line.substr(WITH.size()), ",", with);
			// "Getting with keywords : " + Common::implode(with);
		} else {
			// "Unrecognized setting : " + line;
		}
	}
}

void Playlist::save() {
	const std::string m3uExt = ".m3u";
	if(filepath.size() >= m3uExt.size() && filepath.substr(filepath.size()-m3uExt.size()) != m3uExt) {
		filepath += m3uExt;
	}
	string m3u = filepath;

	ofstream m3u_file(m3u.c_str(), ios::out | ios::trunc);
	m3u_file << HEADER << '\n';
	m3u_file << MUSICMANIAC << '\n';
	m3u_file << ARTISTS << Common::implode(artists) << '\n';
	m3u_file << GENRES << Common::implode(genres) << '\n';
	m3u_file << RATING << rating << '\n';
	m3u_file << MIN_DURATION << minDuration << '\n';
	m3u_file << MAX_DURATION << maxDuration << '\n';
	m3u_file << WITHOUT << Common::implode(without) << '\n';
	m3u_file << WITH << Common::implode(with) << '\n';
	m3u_file << ENDHEADER << '\n';

	for(vector<MusicFile *>::const_iterator i = musics.begin(); i != musics.end(); i++) {
		string filepath = (*i)->getFilepath();
		size_t found = filepath.find_last_of("/");
		m3u_file << INF << (*i)->getDurationInSeconds() << "," << filepath.substr(found+1) << '\n';
		m3u_file << UUID << (*i)->getUUID() << '\n';
		m3u_file << filepath << '\n';
	}
	m3u_file.close();
}

void Playlist::refresh( const std::vector<MusicFile *>& sources) {
	// "Refreshing playlist " + filepath;
	// filtering
	for(std::vector<MusicFile *>::const_iterator i = sources.begin(); i != sources.end(); i++) {
		bool cont = true;
		if (std::find(artists.begin(), artists.end(), (*i)->getArtist()) == artists.end()) {
			//// (*i)->getArtist() + " : is not in artists list : " + implode(artists);
			cont = false;
		}

		const std::vector<std::string> splittedKeywords = (*i)->getSplittedKeywords();
		if(cont) {
			cont = std::find_first_of(splittedKeywords.begin(), splittedKeywords.end(), without.begin(), without.end()) == splittedKeywords.end();
		}

		if(with.size() && cont) {
			cont = std::find_first_of(splittedKeywords.begin(), splittedKeywords.end(), with.begin(), with.end()) != splittedKeywords.end();
		}

		if((*i)->getRating() < rating && cont) {
			//// (*i)->getFilepath() + " : rating does not match : " + Common::toString((*i)->getRating()) + " < " + Common::toString(vrating);
			cont = false;
		}

		unsigned int currentDuration = (*i)->getDurationInSeconds();
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
			// (*i)->getFilepath() +
			   " is not in duration sequence : min = " + Common::toString(min) +
			   " and max = " + Common::toString(max) +
			   " and current = " + Common::toString(currentDuration);
			*/
		}

		if(cont) {
			// "ADDING : " + (*i)->getFilepath();
			add(*i);
		}
	}
	save();
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
