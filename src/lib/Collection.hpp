#pragma once

#include <string>
#include <map>
#include <vector>
#include "MusicFile.hpp"
#include "Playlist.hpp"

#include <boost/filesystem.hpp>

typedef std::map<std::string, Musics > Artists;
typedef std::map<std::string, Musics > Keywords;
typedef std::map<std::string, Musics > Genres;
typedef std::map<std::string, Keywords > KeywordsByArtist;

class Collection {
	public:

        Collection(const std::string& folder, bool regen=false);
		bool factory();
		void refreshPlaylists();
		void generateBest();
		void generateBestByKeyword();
		void consolidateTitles();
		const Musics& getMusics()const;
		const Playlists& getPlaylists()const;
		const Artists& getArtists()const;
		const Keywords& getKeywords()const;
		const Genres& getGenres()const;
		bool valid();
		double progression() const;
		int getTotalCount() const;
		int getReadCount() const;
        void loadFile(const std::string& filepath);
        static MusicFile * getFile(const std::string& filepath);
        void loadAll();

	private:

		void push(MusicFile * );
		boost::filesystem::recursive_directory_iterator iterator;
		const std::string folder;
		unsigned int totalCount;
		unsigned int readCount;
		Musics musics;
		Playlists playlists;
		Artists artists;
		Keywords keywords;
		Genres genres;
		KeywordsByArtist keywordsByArtist;
        static bool regen;
};
