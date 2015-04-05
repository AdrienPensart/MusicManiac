#pragma once

#include "MusicFile.hpp"
#include <taglib/flacfile.h>

class FLACFile : public MusicFile {
	public:

		FLACFile(std::string filepath, TagLib::FLAC::File * flac, bool regen=false);
		virtual void setKeywords(std::string);
		virtual void setRating(double);

	private:

		TagLib::FLAC::File * flac;
};
