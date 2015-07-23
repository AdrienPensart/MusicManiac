#include "db/MusicDb.hpp"
#include "lib/Collection.hpp"
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
	Collection collection(basefolder, false);
	MusicDb db;

	try {
		while(collection.factory()) {
			cout << collection.getReadCount() << " / " << collection.getTotalCount() << " : " << collection.progression()*100 << '\n';
		}
	} catch (boost::filesystem::filesystem_error& fex) {
		cout << "Exception " + std::string(fex.what());
	}

	db.save(collection);
	//collection.generateBest();
	//collection.generateBestByKeyword();
	return 0;
}
