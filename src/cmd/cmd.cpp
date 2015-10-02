#include "db/MusicDb.hpp"
#include "lib/Collection.hpp"
#include "inotify/FileSystemEvent.h"
#include "inotify/Inotify.h"

#include <boost/algorithm/string/predicate.hpp>
using namespace boost::filesystem;

int main(int argc, char * argv[]){
	if(argc < 2){
		return 1;
	}
    std::string basefolder = argv[1];

	Collection collection(basefolder, false);
    collection.loadAll();
	collection.consolidateTitles();
	collection.generateBest();
    /*
	try {
		MusicDb db;
		db.save(collection);
        //db.fetchYoutube();
		//db.generateBestByKeyword();
		//db.generateBest();
	} catch(std::exception& e){
		qDebug() << e.what();
	}
    */
    collection.generateBestByKeyword();
	return 0;
}
