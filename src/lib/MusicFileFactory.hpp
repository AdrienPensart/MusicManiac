#pragma once

#include <vector>
#include <string>
#include "MusicFile.hpp"
#include "Playlist.hpp"

#include <boost/filesystem.hpp>

class MusicFileFactory {
	public:

		MusicFileFactory(const std::string& folder, bool regen=false);
		MusicFile * factory();
		const std::vector<Playlist *>& getPlaylists();
		bool valid();
		double progression();
		int getTotalCount();
		int getReadCount();

	private:

		boost::filesystem::recursive_directory_iterator iterator;
		const std::string folder;
		unsigned int totalCount;
		unsigned int readCount;
		std::vector<Playlist*> playlists;
		bool regen;
};
