#pragma once

#include "MusicFile.hpp"
#include <taglib/mpegfile.h>

class MP3File : public MusicFile {
	public:

		MP3File(std::string filepath, TagLib::MPEG::File * mp3);
		virtual void setKeywords(std::string);
		virtual void setRating(double);
		virtual void setGenre(std::string);
		virtual void setTitle(std::string);
		virtual void setAlbum(std::string);

	private:

		TagLib::MPEG::File * mp3;
};
