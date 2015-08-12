#include "FLACFile.hpp"

#include <common/Utility.hpp>
#include "Uuid.hpp"

#include <taglib/xiphcomment.h>
#include <taglib/tstringlist.h>
#include <taglib/tpropertymap.h>

FLACFile::FLACFile(std::string filepath, TagLib::FLAC::File * _flac, bool regen)
	: MusicFile(filepath, _flac), flac(_flac) {
	double rating = 0;
	if(!flac->xiphComment()->contains("FMPS_RATING")) {
		//// "No rating in file " + filepath;
	} else {
		TagLib::StringList list = flac->xiphComment()->properties()["FMPS_RATING"];
		if(list.size()) {
			std::string ratingStr = list[0].to8Bit(true);
			Common::fromString(ratingStr, rating);
			rating *= 5;

		}
	}
	MusicFile::setRating(rating, false);
	MusicFile::setArtist(flac->xiphComment()->artist().to8Bit(true).c_str(), false);
	MusicFile::setGenre(flac->xiphComment()->genre().to8Bit(true).c_str(), false);
	MusicFile::setTitle(flac->xiphComment()->title().to8Bit(true).c_str(), false);
	MusicFile::setAlbum(flac->xiphComment()->album().to8Bit(true).c_str(), false);

	if(regen || !flac->xiphComment()->contains("UFID")) {
		std::string uuid = newUUID();
		flac->xiphComment()->addField("UFID", uuid.c_str());
		//// "No UFID frame for " + getFilepath() + " generating one : " + uuid;
		MusicFile::setUUID(uuid, true);
	} else {
		TagLib::StringList list = flac->xiphComment()->properties()["UFID"];
		if(list.size()) {
			MusicFile::setUUID(list.front().toCString(), false);
		}
	}

	MusicFile::setKeywords(flac->xiphComment()->comment().to8Bit(true).c_str(), false);
}

void FLACFile::setRating(double _rating) {
	if(!flac->xiphComment()->contains("FMPS_RATING")) {
		flac->xiphComment()->addField("FMPS_RATING", Common::toString(_rating / 5.0).c_str());
	} else {
		flac->xiphComment()->addField("FMPS_RATING", Common::toString(_rating / 5.0).c_str(), true);
	}
	MusicFile::setRating(_rating, true);
}

void FLACFile::setArtist(std::string _artist) {
    flac->xiphComment()->setArtist(_artist.c_str());
    MusicFile::setArtist(_artist, true);
}

void FLACFile::setKeywords(std::string _keywords) {
	flac->xiphComment()->setComment(_keywords.c_str());
	MusicFile::setKeywords(_keywords, true);
}

void FLACFile::setGenre(std::string _genre) {
	flac->xiphComment()->setGenre(_genre.c_str());
	MusicFile::setGenre(_genre, true);
}

void FLACFile::setTitle(std::string _title) {
	flac->xiphComment()->setTitle(_title.c_str());
	MusicFile::setTitle(_title, true);
}

void FLACFile::setAlbum(std::string _album){
	flac->xiphComment()->setAlbum(_album.c_str());
	MusicFile::setAlbum(_album, true);
}
