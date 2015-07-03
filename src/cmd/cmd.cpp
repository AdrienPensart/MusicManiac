#include "lib/MusicFileFactory.hpp"
#include "inotify/FileSystemEvent.h"
#include "inotify/Inotify.h"

#include <iostream>
using namespace std;

int main(int argc, char * argv[]){
	if(argc < 2){
		return 1;
	}
	string basefolder = argv[1];
	MusicFileFactory mff(basefolder, false);
	std::vector<MusicFile*> musics;
	try {
		while(mff.valid()) {
			MusicFile * mf = mff.factory();
			if(mf) {
				musics.push_back(mf);
			}
			cout << mff.getReadCount() << " / " << mff.getTotalCount() << " : " << mff.progression()*100 << '\n';
		}
	} catch (boost::filesystem::filesystem_error& fex) {
		cout << "Exception " + std::string(fex.what());
	}

	std::vector<Playlist *> playlists = mff.getPlaylists();
	for(std::vector<Playlist*>::iterator playlist = playlists.begin(); playlist != playlists.end(); playlist++) {
		(*playlist)->load();
		(*playlist)->refresh(musics);
	}

	boost::filesystem::path dir(argv[1]);

	std::cout << "Setup watches for " << dir <<"..." << std::endl;
	Inotify inotify(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE);
	inotify.watchDirectoryRecursively(dir);

	std::cout << "Waiting for events..." << std::endl;
	while(true){
	  FileSystemEvent event = inotify.getNextEvent();
	  std::cout << "Event wd(" << event.wd << ") " << event.getMaskString() << "for " << event.path << " was triggered!" << std::endl;
	}
	return 0;
}
