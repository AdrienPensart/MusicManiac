#include <taglib/id3v2tag.h>
#include <iostream>
#include <unordered_set>
#include <iomanip>
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

void Collection::generateBest(){
	std::vector<std::string> without;
	without.push_back("cutoff");
	// generate best.m3u playlists
	// mainly for sync purpose
	for (Artists::const_iterator i = artists.begin(); i != artists.end(); i++) {
		Playlist playlist(folder+"/"+i->first+"/best.m3u");
		playlist.setRating(4);
		std::vector<std::string> artists;
		artists.push_back(i->first);
		playlist.setWithout(without);
		playlist.setArtists(artists);
		playlist.refresh(i->second);
		if(!playlist.size()){
			unlink(playlist.getFilepath().c_str());
		} else {
			std::cout << "Generating " << playlist.getFilepath().data() << '\n';
			playlist.save();
		}
	}
}

void Collection::generateBestByKeyword(){
	std::vector<std::string> without;
	without.push_back("cutoff");
	// generate all keywords playlists for each artist
	for (KeywordsByArtist::const_iterator i = keywordsByArtist.begin(); i != keywordsByArtist.end(); i++) {
		for (Keywords::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
			Playlist playlist(folder+"/"+i->first+"/"+j->first+".m3u");
			playlist.setRating(4);
			std::vector<std::string> artists;
			artists.push_back(i->first);
			std::vector<std::string> with;
			with.push_back(j->first);
			playlist.setWith(with);
			playlist.setWithout(without);
			playlist.setArtists(artists);
			playlist.refresh(j->second);

			if(playlist.size() >= 3){
				playlist.save();
				std::cout << "Generating " << playlist.getFilepath().data() << endl;
			}
		}
	}
}

void Collection::consolidateTitles(){
	for(auto music : musics){
		if(!music.second->getTitle().size()){
			music.second->setTitle(music.second->getFilename());
		}
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

void Collection::loadAll(){
    try {
        while(factory()) {
            /*
            cout << getReadCount()
                 << " / " << getTotalCount()
                 << " : " << std::fixed << std::showpoint << std::setprecision(2) << progression()*100 << '\n';
            */
        }
    } catch (boost::filesystem::filesystem_error& fex) {
        cout << "Exception " + std::string(fex.what());
    }
}

void Collection::loadFile(const std::string& filepath){
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

			push(new MP3File(filepath, mp3));
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
				push(new FLACFile(filepath, flac, regen));
			}
		}
	} else if(boost::algorithm::ends_with(filepath, ".m3u")) {
		if(playlists.count(filepath)){
			cout << "Playlist already exists, deleting old..." << filepath << '\n';
			delete playlists[filepath];
		}
		auto playlist = new Playlist(filepath);
		playlist->load();
		playlists[filepath] = playlist;
	} else {
		//// "Music file not supported " + iterator.filePath().toStdString();
	}
}

void Collection::push(MusicFile * music){
	musics[music->getFilepath()] = music;
	artists[music->getArtist()][music->getFilepath()] = music;
	genres[music->getGenre()][music->getFilepath()] = music;
	for(const std::string keyword : music->getSplittedKeywords()){
		keywords[keyword][music->getFilepath()] = music;
		keywordsByArtist[music->getArtist()][keyword][music->getFilepath()] = music;
	}
}

const Artists& Collection::getArtists()const{
	return artists;
}

const Keywords& Collection::getKeywords()const{
	return keywords;
}

const Genres& Collection::getGenres()const{
	return genres;
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
    loadFile(iterator->path().native());
	++iterator;
	return true;
}
