#pragma once

#include <string>
#include <map>
#include <vector>
#include "MusicFile.hpp"
#include "Playlist.hpp"

#include <boost/filesystem.hpp>

typedef std::map<std::string, Musics > Artists;
typedef std::map<std::string, Musics > Albums;
typedef std::map<std::string, Musics > Keywords;
typedef std::map<std::string, Musics > Genres;
typedef std::map<std::string, Keywords > KeywordsByArtist;

typedef std::vector<MusicFile*> MusicVector;
typedef std::vector<std::pair<std::string, MusicVector>> MusicVectorByAlbum;
typedef std::vector<std::pair<std::string, MusicVectorByAlbum>> Tree;

class Collection {
	public:

        Collection();

        void setRegen(bool regen);
        void setRoot(const std::string& folder);
        const std::string& getRoot()const;

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
        static MusicFile * getFile(const std::string& filepath, bool _regen=false);
        void loadAll(bool refresh=false);
        void buildTree();
        Tree& getTree();

	private:

		void push(MusicFile * );
		boost::filesystem::recursive_directory_iterator iterator;
        std::string folder;
		unsigned int totalCount;
		unsigned int readCount;
        Tree tree;
		Musics musics;
		Playlists playlists;
		Artists artists;
		Keywords keywords;
		Genres genres;
		KeywordsByArtist keywordsByArtist;
        bool regen;
};
