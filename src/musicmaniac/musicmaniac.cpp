#include "fs/MusicFileSystem.hpp"

#include "common/Utility.hpp"
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

#include "MainWindow.hpp"
#include <QApplication>
#include <QDebug>

void Usage(ezOptionParser& opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
};

int main(int argc, char * argv[]) try {
    Collection collection;
    /*
    collection.setFolder("/home/crunch/music");
    collection.loadAll();
    Tree tree = collection.getTree();
    for(int i = 0; i < tree.size(); i++){
        cout << tree[i].first << " : " << tree[i].second.size() << " albums\n";
        for(int j = 0; j < tree[i].second.size(); j++){
            cout << "\t" << tree[i].second[j].first << " : " << tree[i].second[j].second.size() << " songs\n";
            for(int h = 0; h < tree[i].second[j].second.size(); h++){
                cout << "\t\t" << tree[i].second[j].second[h]->getTitle() << " at index (" << i << "," << j << "," << h << ")\n";
            }
        }
    }
    collection.generateBest();
    collection.generateBestByKeyword();
    return 0;
    */
    ezOptionParser opt;
    opt.overview = "MusicManiac swiss army knife";
    opt.syntax = "musicmaniac [OPTIONS]";
    opt.example = "\nFill the database with musics : musicmaniac -d music.sqlite -m My_Music_Folder\n"
                  "\nUse the QT GUI : musicmaniac -d music.sqlite --gui";
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

    opt.add("", 0, 0, 0, "Regen UUID", "--regen");
    opt.add("", 0, 1, 0, "FUSE mode", "-f", "--filesystem", "--fuse");
    opt.add("", 0, 1, 0, "Music folder, if not there, start GUI", "-m", "--music");
    opt.add("", 0, 1, 0, "SQLite Database", "-d", "--database", "--db");

    const char ** cargv = const_cast<const char **> (argv);
    opt.parse(argc, cargv);
    if (opt.isSet("-h")) {
        Usage(opt);
        return 1;
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

    std::string dbpath;
    if (opt.isSet("-d")) {
        opt.get("-d")->getString(dbpath);
        cout << "Database selected : " << dbpath << '\n';
    }

    // if no root dir passed, start GUI
    if (opt.isSet("-m")) {
        std::string musicpath;
        opt.get("-m")->getString(musicpath);
        cout << "Music folder selected : " << musicpath << '\n';
        collection.setRoot(musicpath);
        collection.loadAll(true);
    } else {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
    }

    bool regen = false;
    if (opt.isSet("--regen")) {
        regen = true;
        cout << "Regen UUID selected : " << regen << '\n';
        collection.setRegen(regen);
    }

    // filesystem mode
    if (opt.isSet("-f") && (opt.isSet("-m"))) {
        std::string mountpoint;
        opt.get("-f")->getString(mountpoint);

        MusicFileSystem mfs(collection);
        return mfs.run(mountpoint);
    }

    //collection.loadAll();
    //collection.consolidateTitles();
    //collection.generateBest();
    //collection.generateBestByKeyword();

    //MusicDb db(dbpath.c_str());
    //db.save(collection);
    //db.fetchYoutube();
    //db.generateBestByKeyword();
    //db.generateBest();

    /*
    string line;
    while (getline(cin, line)) {
        std::vector<std::string> output;
        Common::split(line, ":", output);
        if(output.size() != 2){
            qDebug() << "Invalid input";
        }

        std::vector<std::string> events;
        Common::split(output[0], ",", events);
        for(auto event : events){
            if(event == "DELETE" ||
               event == "CREATE" ||
               event == "CLOSE_WRITE"){
            }
        }

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
    */
} catch(std::exception& e){
    qDebug() << e.what();
    return 0;
}
