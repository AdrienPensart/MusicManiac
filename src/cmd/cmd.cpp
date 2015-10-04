#include "db/MusicDb.hpp"
#include "lib/Collection.hpp"
#include "inotify/FileSystemEvent.h"
#include "inotify/Inotify.h"
#include "json/src/json.hpp"
using json = nlohmann::json;

#include <boost/algorithm/string/predicate.hpp>
using namespace boost::filesystem;

#include "ezOptionParser.hpp"
using namespace ez;

#include <string>
#include <vector>
#include <iostream>
using namespace std;

void Usage(ezOptionParser& opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
};

int main(int argc, const char * argv[]){
    ezOptionParser opt;
    opt.overview = "MusicManiac swiss army knife";
    opt.syntax = "musicmaniac [OPTIONS]";
    opt.example = "musicmaniac -d music.sqlite -m My_Music_Folder\n\n";
    opt.footer = "musicmaniac v1 Copyright (C) 2015 Adrien Pensart\nThis program is free and without warranty.\n";

    opt.add(
        "", // Default.
        0, // Required?
        0, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Display usage instructions.", // Help description.
        "-h",     // Flag token.
        "-help",  // Flag token.
        "--help", // Flag token.
        "--usage" // Flag token.
    );

    opt.add(
        "",
        0,
        0,
        0,
        "Start GUI",
        "-g",
        "--gui"
    );

    opt.add(
        "",
        0,
        1,
        0,
        "Music folder",
        "-m",
        "--music"
    );

    opt.add(
        "",
        0,
        1,
        0,
        "SQLite Database"
        "-d",
        "--database",
        "--db"
    );

    opt.parse(argc, argv);
    if (opt.isSet("-h")) {
        Usage(opt);
        return 1;
    }

    if (opt.isSet("-g")) {
        cout << "Starting GUI.\n";
        return 0;
    }

    std::vector<std::string> badOptions;
    int i;
    if(!opt.gotRequired(badOptions)) {
        for(i=0; i < badOptions.size(); ++i)
            std::cerr << "ERROR: Missing required option " << badOptions[i] << ".\n\n";
        Usage(opt);
        return 1;
    }

    if(!opt.gotExpected(badOptions)) {
        for(i=0; i < badOptions.size(); ++i)
            std::cerr << "ERROR: Got unexpected number of arguments for option " << badOptions[i] << ".\n\n";
        Usage(opt);
        return 1;
    }

    try {
        std::string dbpath;
        if (opt.isSet("-d")) {
            opt.get("-d")->getString(dbpath);
            cout << "Database selected : " << dbpath << '\n';
        }

        if (opt.isSet("-m")) {
            std::string musicpath;
            opt.get("-m")->getString(musicpath);
            cout << "Music folder selected : " << musicpath << '\n';

            Collection collection(musicpath, false);
            collection.loadAll();
            collection.consolidateTitles();
            collection.generateBest();
            collection.generateBestByKeyword();

            //MusicDb db(dbpath.c_str());
            //db.save(collection);
            //db.fetchYoutube();
            //db.generateBestByKeyword();
            //db.generateBest();
        } else {
            string filepath;
            while (getline(cin, filepath)) {
                MusicFile * file = Collection::getFile(filepath);
                if(file){
                    json j;

                    j["title"] = file->getTitle();
                    j["artist"] = file->getArtist();
                    j["filepath"] = file->getFilepath();
                    j["genre"] = file->getGenre();
                    j["album"] = file->getAlbum();
                    j["rating"] = file->getRating();
                    j["keywords"] = file->getSplittedKeywords();
                    j["uuid"] = file->getUUID();
                    j["duration"] = file->getDurationInSeconds();

                    std::cout << j.dump(4) << std::endl;
                }
            }
        }
	} catch(std::exception& e){
		qDebug() << e.what();
	}
	return 0;
}
