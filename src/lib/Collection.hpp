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
        ~Collection();
        void setRegen(bool regen);
        void setRoot(const std::string& folder);
        const std::string& getRoot()const;

		bool factory();
        void addPlaylist(Playlist *);
        void refreshPlaylist(Playlist *);
        void generateList();
        void generatePlaylists();
		void generateBest();
		void generateBestByKeyword();
		void consolidateTitles();

        std::set<std::string> getArtists() const;
        std::set<std::string> getKeywords() const;
        std::set<std::string> getGenres() const;

        const Musics& getMusics() const;
        const Playlists& getAllPlaylists() const;
        const Playlists& getManualPlaylists() const;
        const Playlists& getAutogenPlaylists() const;
        const PlaylistsByArtist& getManualPlaylistsByArtist() const;
        const PlaylistsByArtist& getAutoPlaylistsByArtist() const;

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
        void load();

	private:

		void push(MusicFile * );
		boost::filesystem::recursive_directory_iterator iterator;
        std::string folder;
		unsigned int totalCount;
		unsigned int readCount;
		Musics musics;

        // references all loaded playlists
        Playlists allPlaylists;
        // store manual playlists (more than one artist)
        Playlists manualPlaylists;
        // store manual playlists that should be autoregen and with more than one artist in it
        Playlists autogenPlaylists;

        MusicsByArtistAlbums musicsByArtistsAlbums;
        MusicsByArtists musicsByArtists;
        MusicsByKeywords musicsByKeywords;
        MusicsByGenres musicsByGenres;
		KeywordsByArtist keywordsByArtist;

        PlaylistsByArtist manualPlaylistsByArtist;
        PlaylistsByArtist autoPlaylistsByArtist;

        bool regen;
};
