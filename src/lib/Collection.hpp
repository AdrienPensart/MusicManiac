#pragma once

#include <string>
#include <vector>
#include "MusicFile.hpp"
#include "Playlist.hpp"

#include <boost/filesystem.hpp>

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
        const PlaylistsByArtist& getPlaylistsByArtist()const;
		bool valid();
        double progression() const;
		int getTotalCount() const;
		int getReadCount() const;
        void loadFile(const std::string& filepath);
        static MusicFile * getFile(const std::string& filepath, bool _regen=false);
        void load(bool refresh=false);
        Tree buildTree();
        Tree buildFilterTree(Playlist& filter);

	private:

        void build(Tree& tree, MusicFile * music);
		void push(MusicFile * );
		boost::filesystem::recursive_directory_iterator iterator;
        std::string folder;
		unsigned int totalCount;
		unsigned int readCount;
		Musics musics;
		Playlists playlists;
		Artists artists;
        Keywords keywords;
		Genres genres;
		KeywordsByArtist keywordsByArtist;
        PlaylistsByArtist playlistsByArtist;
        bool regen;
};
