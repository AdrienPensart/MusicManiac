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
namespace fs = boost::filesystem;
namespace bl = boost::lambda;

Collection::Collection() :
    regen(false),
	totalCount(0),
    readCount(0) {
}

void Collection::setRegen(bool _regen){
    regen = _regen;
}

void Collection::setRoot(const std::string& _folder){
    if (!fs::is_directory(_folder)){
        std::cout << _folder << " is not a valid directory\n";
        return;
    }
    folder = _folder;
    for(fs::recursive_directory_iterator it(folder); it != fs::recursive_directory_iterator(); ++it) {
        totalCount++;
    }
    // "Total count of files in " + folder + " : " + Common::toString(totalCount);
    iterator = fs::recursive_directory_iterator(folder);
}

const std::string& Collection::getRoot() const {
    return folder;
}

void Collection::generateBest(){
	std::vector<std::string> without;
	without.push_back("cutoff");
	// generate best.m3u playlists
	// mainly for sync purpose
    for (MusicsByArtists::const_iterator i = musicsByArtists.begin(); i != musicsByArtists.end(); i++) {
		Playlist playlist(folder+"/"+i->first+"/best.m3u");
		playlist.setRating(4);
		std::vector<std::string> artists;
        artists.push_back(i->first);
		playlist.setWithout(without);
		playlist.setArtists(artists);
        playlist.refreshWith(i->second);
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
        for (MusicsByKeywords::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
			Playlist playlist(folder+"/"+i->first+"/"+j->first+".m3u");
			playlist.setRating(4);
			std::vector<std::string> artists;
			artists.push_back(i->first);
			std::vector<std::string> with;
			with.push_back(j->first);
			playlist.setWith(with);
			playlist.setWithout(without);
			playlist.setArtists(artists);
            playlist.refreshWith(j->second);

			if(playlist.size() >= 3){
				playlist.save();
				std::cout << "Generating " << playlist.getFilepath().data() << endl;
			}
		}
	}
    /*
    // generate 2 tags playlist
    for (KeywordsByArtist::const_iterator i = keywordsByArtist.begin(); i != keywordsByArtist.end(); i++) {
        for (Keywords::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
            for (Keywords::const_iterator h = i->second.begin(); h != i->second.end(); h++) {
                if(j == h){
                    continue;
                }
                Playlist playlist(folder+"/"+i->first+"/"+j->first+"_"+h->first+".m3u");
                playlist.setRating(4);
                std::vector<std::string> artists;
                artists.push_back(i->first);
                std::vector<std::string> with;
                with.push_back(j->first);
                with.push_back(h->first);
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
    */
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
    return iterator != fs::recursive_directory_iterator();
}

const Playlists& Collection::getPlaylists() const {
	return playlists;
}

const Musics& Collection::getMusics() const{
	return musics;
}

/*
Tree Collection::buildTree() {
    Tree tree;
    for(auto music_pair : musics){
        MusicFile * music = music_pair.second;
        build(tree, music);
    }
    return tree;
}

Tree Collection::buildFilterTree(Playlist& filter){
    Tree tree;
    for(auto music_pair : musics){
        MusicFile * music = music_pair.second;
        if(filter.conform(music)){
            build(tree, music);
        }
    }
    return tree;
}
*/

void Collection::load(bool refresh){
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

    if(refresh){
        refreshPlaylists();
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

void Collection::loadFile(const std::string& filepath){ 
    if(boost::algorithm::ends_with(filepath, ".m3u")) {
		if(playlists.count(filepath)){
			cout << "Playlist already exists, deleting old..." << filepath << '\n';
			delete playlists[filepath];
		}
		auto playlist = new Playlist(filepath);
		playlist->load();
		playlists[filepath] = playlist;

        auto artists = playlist->getArtists();
        for(const auto& artist : artists)
        {
            playlistsByArtist[artist].push_back(playlist);
        }
    } else {
        MusicFile * file = getFile(filepath, regen);
        if(musics.count(filepath)){
            cout << "Music already exists, deleting old..." << filepath << '\n';
            delete musics[filepath];
        }
        push(file);
    }
}

MusicFile * Collection::getFile(const std::string& filepath, bool _regen) {
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
            } else if(_regen) {
                tag->removeFrames("UFID");
                mp3->save();
                // reopen file
                delete mp3;
                mp3 = new TagLib::MPEG::File(filepath.c_str());
            }
            return new MP3File(filepath, mp3);
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
                return new FLACFile(filepath, flac, _regen);
            }
        }
    }
    return 0;
}

void Collection::push(MusicFile * music){
    if(music){
        musicsByArtistsAlbums[music->getArtist()][music->getAlbum()][music->getTitle()] = music;
        musics[music->getFilepath()] = music;
        musicsByArtists[music->getArtist()][music->getFilepath()] = music;
        musicsByGenres[music->getGenre()][music->getFilepath()] = music;
        for(auto keyword : music->getSplittedKeywords()){
            musicsByKeywords[keyword][music->getFilepath()] = music;
            keywordsByArtist[music->getArtist()][keyword][music->getFilepath()] = music;
        }
    }
}

const MusicsByArtists& Collection::getMusicsByArtists() const {
    return musicsByArtists;
}

const MusicsByKeywords& Collection::getMusicsByKeywords() const {
    return musicsByKeywords;
}

const MusicsByGenres& Collection::getMusicsByGenres() const {
    return musicsByGenres;
}

const PlaylistsByArtist& Collection::getPlaylistsByArtist() const {
    return playlistsByArtist;
}

const MusicsByArtistAlbums& Collection::getMusicsByArtistsAlbums() const {
    return musicsByArtistsAlbums;
}

void Collection::refreshPlaylists(){
	for(Playlists::iterator playlist = playlists.begin(); playlist != playlists.end(); playlist++) {
		playlist->second->load();
        playlist->second->refreshWith(musics);
        playlist->second->save();
	}
}
