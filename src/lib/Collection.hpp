#pragma once

#include <string>
#include <vector>
#include <set>
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
        std::set<std::string> getKeywords() const;
        const Musics& getMusics() const;
        const Playlists& getPlaylists() const;
        const MusicsByArtistAlbums& getMusicsByArtistsAlbums() const;
        const MusicsByArtists& getMusicsByArtists() const;
        const MusicsByKeywords& getMusicsByKeywords() const;
        const MusicsByGenres& getMusicsByGenres() const;
        const PlaylistsByArtist& getPlaylistsByArtist() const;
        const MusicsByAlbums& getMusicsByAlbums() const;
		bool valid();
        double progression() const;
		int getTotalCount() const;
		int getReadCount() const;
        void loadFile(const std::string& filepath);
        static MusicFile * getFile(const std::string& filepath, bool _regen=false);
        void load(bool refresh=false);

	private:

		void push(MusicFile * );
		boost::filesystem::recursive_directory_iterator iterator;
        std::string folder;
		unsigned int totalCount;
		unsigned int readCount;
		Musics musics;
        Playlists playlists;
        MusicsByArtistAlbums musicsByArtistsAlbums;
        MusicsByArtists musicsByArtists;
        MusicsByKeywords musicsByKeywords;
        MusicsByGenres musicsByGenres;
		KeywordsByArtist keywordsByArtist;
        PlaylistsByArtist playlistsByArtist;
        bool regen;
};
