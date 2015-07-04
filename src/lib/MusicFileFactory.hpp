#pragma once

#include <string>
#include "MusicFile.hpp"
#include "Playlist.hpp"

#include <boost/filesystem.hpp>

class MusicFileFactory {
	public:

		MusicFileFactory(const std::string& folder, bool regen=false);
		bool factory();
		void refreshPlaylists();
		const Musics& getMusics()const;
		const Playlists& getPlaylists()const;
		bool valid();
		double progression() const;
		int getTotalCount() const;
		int getReadCount() const;
		void load(const std::string& filepath);

	private:

		boost::filesystem::recursive_directory_iterator iterator;
		const std::string folder;
		unsigned int totalCount;
		unsigned int readCount;
		Musics musics;
		Playlists playlists;
		bool regen;
};
