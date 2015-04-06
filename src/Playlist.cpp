#include "Playlist.hpp"
#include <common/Utility.hpp>

#include <fstream>
#include <sstream>
#include <iterator>
using namespace std;

const std::string HEADER = "#EXTM3U";
const std::string MUSICMANIAC = "#EXTREM:musicmaniac";
const std::string ARTISTS = "#EXTREM:artists:";
const std::string RATING = "#EXTREM:rating:";
const std::string MIN_DURATION = "#EXTREM:minDuration:";
const std::string MAX_DURATION = "#EXTREM:maxDuration:";
const std::string WITHOUT = "#EXTREM:without:";
const std::string WITH = "#EXTREM:with:";
const std::string INF = "#EXTINF: ";
const std::string UUID = "#EXTREM:uuid ";

string implode(const std::vector<std::string>& strings, const std::string& delim = ",") {
	std::ostringstream imploded;
	std::copy(strings.begin(), strings.end(), std::ostream_iterator<std::string>(imploded, delim.c_str()));
	std::string str = imploded.str();
	return str.substr(0, str.size()-1);
}

Playlist::Playlist(std::string _filepath) :
	filepath(_filepath) {
}

void Playlist::load() {
	ifstream playlist(filepath.c_str());
	std::string header;
	std::string musicmaniac;
	std::getline(playlist, header);
	std::getline(playlist, musicmaniac);
	if(header != HEADER || musicmaniac != MUSICMANIAC) {
		LOG << "Not a MusicManiac playlist";
		return;
	}

	std::string artistsLine;
	std::getline(playlist, artistsLine);
	if(artistsLine.find_first_of(ARTISTS) == string::npos) {
		LOG << "Invalid playlist : no artists";
		return;
	}
	Common::split(artistsLine.substr(ARTISTS.size()), ",", artists);
	LOG << "Getting artists : " + implode(artists);

	std::string ratingLine;
	std::getline(playlist, ratingLine);
	if(ratingLine.find_first_of(RATING) == std::string::npos) {
		LOG << "Invalid playlist : no rating";
		return;
	}
	Common::fromString(ratingLine.substr(RATING.size()), rating);
	LOG << "Getting rating : " + Common::toString(rating);

	std::string minDurationLine;
	std::getline(playlist, minDurationLine);
	if(minDurationLine.find_first_of(MIN_DURATION) == std::string::npos) {
		LOG << "Invalid playlist : no min duration";
		return;
	}
	minDuration = minDurationLine.substr(MIN_DURATION.size());
	LOG << "Getting min duration : " + minDuration;

	std::string maxDurationLine;
	std::getline(playlist, maxDurationLine);
	if(maxDurationLine.find_first_of(MAX_DURATION) == std::string::npos) {
		LOG << "Invalid playlist : no max duration";
		return;
	}
	maxDuration = maxDurationLine.substr(MAX_DURATION.size());
	LOG << "Getting max duration : " + maxDuration;

	std::string withoutLine;
	std::getline(playlist, withoutLine);
	if(withoutLine.find_first_of(WITHOUT) == std::string::npos) {
		LOG << "Invalid playlist : no without keywords";
		return;
	}
	Common::split(withoutLine.substr(WITHOUT.size()), ",", without);
	LOG << "Getting without keywords : " + implode(without);

	std::string withLine;
	std::getline(playlist, withLine);
	if(withLine.find_first_of(WITH) == std::string::npos) {
		LOG << "Invalid playlist : no with keywords";
		return;
	}
	Common::split(withLine.substr(WITH.size()), ",", with);
	LOG << "Getting with keywords : " + implode(with);
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
	m3u_file << ARTISTS << implode(artists) << '\n';
	m3u_file << RATING << rating << '\n';
	m3u_file << MIN_DURATION << minDuration << '\n';
	m3u_file << MAX_DURATION << maxDuration << '\n';
	m3u_file << WITHOUT << implode(without) << '\n';
	m3u_file << WITH << implode(with) << '\n';

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
	LOG << "Refreshing playlist " + filepath;
	// filtering
	for(std::vector<MusicFile *>::const_iterator i = sources.begin(); i != sources.end(); i++) {
		bool cont = true;
		if (std::find(artists.begin(), artists.end(), (*i)->getArtist()) == artists.end()) {
			//LOG << (*i)->getArtist() + " : is not in artists list : " + implode(artists);
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
			//LOG << (*i)->getFilepath() + " : rating does not match : " + Common::toString((*i)->getRating()) + " < " + Common::toString(vrating);
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
			LOG << (*i)->getFilepath() +
			   " is not in duration sequence : min = " + Common::toString(min) +
			   " and max = " + Common::toString(max) +
			   " and current = " + Common::toString(currentDuration);
			*/
		}

		if(cont) {
			LOG << "ADDING : " + (*i)->getFilepath();
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

const std::vector<std::string>& Playlist::getArtists(){
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

const std::vector<std::string>& Playlist::getWith(){
    return with;
}

void Playlist::setWith(const std::vector<std::string>& _with) {
	with = _with;
}

const std::vector<std::string>& Playlist::getWithout(){
    return without;
}

void Playlist::setWithout(const std::vector<std::string>& _without) {
	without = _without;
}
