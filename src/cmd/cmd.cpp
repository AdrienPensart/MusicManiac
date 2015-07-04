#include "lib/MusicFileFactory.hpp"
#include "inotify/FileSystemEvent.h"
#include "inotify/Inotify.h"

#include <boost/algorithm/string/predicate.hpp>
using namespace boost::filesystem;
#include <iostream>
#include <map>
using namespace std;

int main(int argc, char * argv[]){
	if(argc < 2){
		return 1;
	}
	string basefolder = argv[1];
	MusicFileFactory mff(basefolder, false);
#include <boost/algorithm/string/predicate.hpp>
	try {
		while(mff.factory()) {
			cout << mff.getReadCount() << " / " << mff.getTotalCount() << " : " << mff.progression()*100 << '\n';
		}
	} catch (boost::filesystem::filesystem_error& fex) {
		cout << "Exception " + std::string(fex.what());
	}
	mff.refreshPlaylists();

	boost::filesystem::path dir(argv[1]);
	std::cout << "Setup watches for " << dir <<"..." << std::endl;
	Inotify inotify(IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE);
	inotify.watchDirectoryRecursively(dir);

	std::cout << "Waiting for events..." << std::endl;
	while(true){
		FileSystemEvent event = inotify.getNextEvent();
		std::cout << "Event wd(" << event.wd << ") " << event.getMaskString() << "for " << event.path << " was triggered!\n";
		if(boost::algorithm::ends_with(event.path.native(), ".m3u")){
			continue;
		}
		mff.load(event.path.native());
		mff.refreshPlaylists();
	}
	return 0;
}
