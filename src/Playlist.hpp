#pragma once

#include <vector>
#include <string>
#include "MusicFile.hpp"

class Playlist {
	public:

		Playlist(std::string filepath);
		void add(MusicFile * file);
		void load();
		void refresh(const std::vector<MusicFile *>& sources);
		void save();

		double getRating();
		void setRating(double _rating);
		std::string getMaxDuration();
		void setMaxDuration(const std::string& _maxDuration);
		std::string getMinDuration();
		void setMinDuration(const std::string& _minDuration);
        const std::vector<std::string>& getWith();
		void setWith(const std::vector<std::string>& _with);
        const std::vector<std::string>& getWithout();
		void setWithout(const std::vector<std::string>& _without);
        const std::vector<std::string>& getArtists();
		void setArtists(const std::vector<std::string>& _artists);

	private:

		std::string filepath;
		double rating;
		std::string minDuration;
		std::string maxDuration;
		std::vector<std::string> artists;
		std::vector<std::string> without;
		std::vector<std::string> with;
		std::vector<MusicFile *> musics;
};
