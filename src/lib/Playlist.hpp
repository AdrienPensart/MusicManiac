#pragma once

#include <string>
#include <map>
#include "MusicFile.hpp"
#include "common/Utility.hpp"

class Playlist {
	public:

		Playlist(const std::string& filepath);
		void add(MusicFile * file);
		void load();
        bool conform(MusicFile * music);
		void refresh(Musics musics);
		void save();
		size_t size()const;

		std::string getFilename() const;
		const std::string& getFilepath()const;
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
		const std::vector<std::string>& getGenres();
		void setGenres(const std::vector<std::string>& _genres);

	private:

		bool valid;
		std::string filecontent;
		std::string filepath;
		double rating;
		std::string minDuration;
		std::string maxDuration;
		std::vector<std::string> genres;
		std::vector<std::string> artists;
		std::vector<std::string> without;
		std::vector<std::string> with;
		Musics musics;
};

typedef std::map<std::string, Playlist*> Playlists;
