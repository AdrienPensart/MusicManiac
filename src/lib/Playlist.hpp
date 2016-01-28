#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>
#include "MusicFile.hpp"
#include "common/Utility.hpp"

class Playlist;

typedef std::map<std::string, Playlist*> Playlists;
typedef std::map<std::string, Playlists> PlaylistsByArtist;

class Playlist {

	public:

		Playlist(const std::string& filepath);
		void add(MusicFile * file);
		void load();
        bool conform(MusicFile * music);
        void refreshWith(const Musics& musics);
		void save();
        const Musics& getMusics();
		size_t size()const;

		std::string getFilename() const;
		const std::string& getFilepath()const;
		double getRating();
        double getAverageRating();
		void setRating(double _rating);
		std::string getMaxDuration();
		void setMaxDuration(const std::string& _maxDuration);
		std::string getMinDuration();
		void setMinDuration(const std::string& _minDuration);
        std::string getDuration();
        const std::set<std::string>& getWith();
        void setWith(const std::set<std::string>& _with);
        const std::set<std::string>& getWithout();
        void setWithout(const std::set<std::string>& _without);
        const std::set<std::string>& getArtists();
        void setArtists(const std::set<std::string>& _artists);
        const std::set<std::string>& getGenres();
        void setGenres(const std::set<std::string>& _genres);

	private:

		bool valid;
		std::string filecontent;
		std::string filepath;
		double rating;
		std::string minDuration;
		std::string maxDuration;
        std::set<std::string> genres;
        std::set<std::string> artists;
        std::set<std::string> without;
        std::set<std::string> with;
		Musics musics;
};
