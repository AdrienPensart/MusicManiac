#include <taglib/id3v2tag.h>
#include <iostream>
#include <unordered_set>
using namespace std;

#include "Collection.hpp"
#include "MP3File.hpp"
#include "FLACFile.hpp"
#include "Debugger.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lambda/bind.hpp>
using namespace boost::filesystem;
using namespace boost::lambda;

Collection::Collection(const std::string& _folder, bool _regen) :
	folder(_folder),
	iterator(_folder),
	regen(_regen),
	totalCount(0),
	readCount(0) {
	// "Constructing directory " + folder;
	try {
		for(recursive_directory_iterator it(folder); it != recursive_directory_iterator(); ++it) {
			totalCount++;
		}
		// "Total count of files in " + folder + " : " + Common::toString(totalCount);
	} catch (boost::filesystem::filesystem_error& fex) {
		// "Exception " + std::string(fex.what());
	}
}

int Collection::getTotalCount() const {
	return totalCount;
}

int Collection::getReadCount() const {
	return readCount;
}

double Collection::progression() const {
	return (double)readCount / (double)totalCount;
}

bool Collection::valid() {
	return iterator != recursive_directory_iterator();
}

const Playlists& Collection::getPlaylists() const {
	return playlists;
}

const Musics& Collection::getMusics() const{
	return musics;
}

void Collection::load(const std::string& filepath){
	if(boost::algorithm::ends_with(filepath, ".mp3")) {
		TagLib::MPEG::File * mp3 = new TagLib::MPEG::File(filepath.c_str());
		if(!mp3->audioProperties()) {
			// "No audio property";
		} else if(!mp3->hasID3v2Tag()) {
			// "No ID3v2 Tag present";
		} else {
			TagLib::ID3v2::Tag * tag = mp3->ID3v2Tag();
			if(!tag) {
				// "Tag invalid";
			} else if(regen) {
				tag->removeFrames("UFID");
				mp3->save();
				// reopen file
				delete mp3;
				mp3 = new TagLib::MPEG::File(filepath.c_str());
			}
			if(musics.count(filepath)){
				cout << "Music already exists, deleting old..." << filepath << '\n';
				delete musics[filepath];
			}
			musics[filepath] = new MP3File(filepath, mp3);
		}
	} else if(boost::algorithm::ends_with(filepath, ".flac")) {
		TagLib::FLAC::File * flac = new TagLib::FLAC::File(filepath.c_str());
		if(!flac->audioProperties()) {
			// "No audio property";
		} else if(!flac->hasXiphComment()) {
			// "No XiphComment present";
		} else {
			TagLib::Ogg::XiphComment * tag = flac->xiphComment();
			if(!tag) {
				// "Tag invalid";
			} else {
				if(musics.count(filepath)){
					cout << "Music already exists, deleting old..." << filepath << '\n';
					delete musics[filepath];
				}
				musics[filepath] = new FLACFile(filepath, flac, regen);
			}
		}
	} else if(boost::algorithm::ends_with(filepath, ".m3u")) {
		if(playlists.count(filepath)){
			cout << "Playlist already exists, deleting old..." << filepath << '\n';
			delete playlists[filepath];
		}
		playlists[filepath] = new Playlist(filepath);
	} else {
		//// "Music file not supported " + iterator.filePath().toStdString();
	}
}

void Collection::refreshPlaylists(){
	for(Playlists::iterator playlist = playlists.begin(); playlist != playlists.end(); playlist++) {
		playlist->second->load();
		playlist->second->refresh(musics);
	}
}

bool Collection::factory() {
	if(!valid()) {
		return false;
	}
	readCount++;
	Debugger::instance().setCurrentMusic(iterator->path().native());
	load(iterator->path().native());
	++iterator;
	return true;
}
