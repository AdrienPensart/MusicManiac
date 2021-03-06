#include "Uuid.hpp"
#include <common/Utility.hpp>
#include "MP3File.hpp"

#include <taglib/tpropertymap.h>
#include <taglib/id3v2tag.h>
#include <taglib/uniquefileidentifierframe.h>
#include <taglib/textidentificationframe.h>

MP3File::MP3File(std::string filepath, TagLib::MPEG::File * _mp3)
	: MusicFile(filepath, _mp3), mp3(_mp3) {
	double rating = 0;
	TagLib::StringList list = mp3->properties()["FMPS_RATING"];
	if(list.size() == 1) {
		std::string ratingStr = list[0].to8Bit(true);
		Common::fromString(ratingStr, rating);
		rating *= 5;
	}

	MusicFile::setRating(rating, false);
	MusicFile::setKeywords(mp3->ID3v2Tag()->comment().to8Bit(true).c_str(), false);
	MusicFile::setArtist(mp3->ID3v2Tag()->artist().to8Bit(true).c_str(), false);
	MusicFile::setGenre(mp3->ID3v2Tag()->genre().to8Bit(true).c_str(), false);
	MusicFile::setTitle(mp3->ID3v2Tag()->title().to8Bit(true).c_str(), false);
	MusicFile::setAlbum(mp3->ID3v2Tag()->album().to8Bit(true).c_str(), false);

	// use first frame
	const TagLib::ID3v2::FrameListMap& frames = mp3->ID3v2Tag()->frameListMap();
	TagLib::ID3v2::FrameListMap::ConstIterator ufidIter = frames.find("UFID");
	std::string uuid = newUUID();
	if(ufidIter == frames.end()) {
		//// "No UFID frame for " + getFilepath() + " generating one " + uuid;
		TagLib::ID3v2::UniqueFileIdentifierFrame * ufid = new TagLib::ID3v2::UniqueFileIdentifierFrame("braincraft", uuid.c_str());
		mp3->ID3v2Tag()->addFrame(ufid);
		MusicFile::setUUID(uuid, true);
	} else {
		TagLib::ID3v2::UniqueFileIdentifierFrame * ufid = (TagLib::ID3v2::UniqueFileIdentifierFrame *)ufidIter->second.front();
		uuid = std::string(ufid->identifier().data(), 36);
		MusicFile::setUUID(uuid, false);
	}
}

void MP3File::setRating(double _rating){
	MusicFile::setRating(_rating, true);
	TagLib::StringList& list = mp3->properties()["FMPS_RATING"];
	if(list.size() == 1) {
		list[0] = Common::toString(_rating / 5.0).c_str();
	}
}

void MP3File::setKeywords(std::string _keywords){
	mp3->ID3v2Tag()->setComment(_keywords.c_str());
	MusicFile::setKeywords(_keywords, true);
}

void MP3File::setArtist(std::string _artist){
    mp3->ID3v2Tag()->setArtist(_artist.c_str());
    MusicFile::setArtist(_artist, true);
}

void MP3File::setGenre(std::string _genre){
	mp3->ID3v2Tag()->setGenre(_genre.c_str());
	MusicFile::setGenre(_genre, true);
}

void MP3File::setTitle(std::string _title){
	mp3->ID3v2Tag()->setTitle(_title.c_str());
	MusicFile::setTitle(_title, true);
}

void MP3File::setAlbum(std::string _album){
	mp3->ID3v2Tag()->setAlbum(_album.c_str());
	MusicFile::setAlbum(_album, true);
}
