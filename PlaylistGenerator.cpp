#include "PlaylistGenerator.hpp"
#include <common/Utility.hpp>

#include <fstream>
#include <sstream>
#include <iterator>
using namespace std;

string implode(const std::vector<std::string> strings){
    const char* const delim = ",";
    std::ostringstream imploded;
    std::copy(strings.begin(), strings.end(), std::ostream_iterator<std::string>(imploded, delim));
    std::string str = imploded.str();
    return str.substr(0, str.size()-1);
}

PlaylistGenerator::PlaylistGenerator() {
}

void PlaylistGenerator::add(MusicFile * music) {
    musics.push_back(music);
}

void PlaylistGenerator::save(std::string filepath){
    string m3u = filepath;
    ofstream m3u_file(m3u.c_str(), ios::out | ios::trunc);
    m3u_file << "#EXTM3U\n";
    m3u_file << "#EXTREM:rating " << rating << '\n';
    m3u_file << "#EXTREM:minDuration " << minDuration << '\n';
    m3u_file << "#EXTREM:maxDuration " << maxDuration << '\n';
    m3u_file << "#EXTREM:without " << implode(without) << '\n';
    m3u_file << "#EXTREM:with " << implode(with) << '\n';

    for(vector<MusicFile *>::const_iterator i = musics.begin(); i != musics.end(); i++){
        string filepath = (*i)->getFilepath();
        size_t found = filepath.find_last_of("/");
        m3u_file << "#EXTINF: " << (*i)->getDurationInSeconds() << "," << filepath.substr(found+1) << '\n';
        m3u_file << "#EXTREM:uuid " << (*i)->getUUID() << '\n';
        m3u_file << filepath.substr(basefolder.size()+1) << '\n';
    }
    m3u_file.close();
}

void PlaylistGenerator::setBasefolder(std::string _basefolder){
    basefolder = _basefolder;
}

void PlaylistGenerator::setRating(const std::string& _rating){
    rating = _rating;
}

void PlaylistGenerator::setMaxDuration(const std::string& _maxDuration){
    maxDuration = _maxDuration;
}

void PlaylistGenerator::setMinDuration(const std::string& _minDuration){
    minDuration = _minDuration;
}

void PlaylistGenerator::setWith(const std::vector<std::string>& _with){
    with = _with;
}

void PlaylistGenerator::setWithout(const std::vector<std::string>& _without){
    without = _without;
}

void PlaylistGenerator::refresh(std::string filepath, const std::vector<MusicFile *>& sources){
    LOG << "Refreshing playlist " + filepath;

    ifstream playlist(filepath.c_str());
    std::string line;
    std::getline(playlist, line); // consume #EXTM3U

    std::vector<std::string> ratingLine;
    std::getline(playlist, line); // consume #EXTREM:rating
    Common::split(line, " ", ratingLine);
    rating = ratingLine[1];
    LOG << "Getting rating " + rating;

    std::vector<std::string> minDurationLine;
    std::getline(playlist, line); // consume #EXTREM:minDuration
    Common::split(line, " ", minDurationLine);
    minDuration = minDurationLine[1];
    LOG << "Getting min duration " + minDuration;

    std::vector<std::string> maxDurationLine;
    std::getline(playlist, line); // consume #EXTREM:maxDuration
    Common::split(line, " ", maxDurationLine);
    maxDuration = maxDurationLine[1];
    LOG << "Getting max duration " + maxDuration;

    std::vector<std::string> withoutLine;
    std::getline(playlist, line); // consume #EXTREM:without
    Common::split(line, " ", withoutLine);
    Common::split(withoutLine[1], ",", without);
    LOG << "Getting without keywords " + withoutLine[1];

    std::vector<std::string> withLine;
    std::getline(playlist, line); // consume #EXTREM:with
    Common::split(line, " ", withLine);
    Common::split(withLine[1], ",", with);
    LOG << "Getting with keywords " + withLine[1];

    const std::string prefix = "#EXTREM:uuid ";
    while(std::getline(playlist, line)){
        if(line.substr(0, prefix.size()) != prefix) {
            continue;
        }
        line = line.substr(prefix.size());
        LOG << "Reading UUID : " + line;
        bool found = false;
        for(std::vector<MusicFile *>::const_iterator i = sources.begin(); i != sources.end(); i++){
            string uuid = (*i)->getUUID();
            if(uuid == line){
                add(*i);
                LOG << "Music with UUID " + uuid + " found in library";
                found = true;
                break;
            }
        }
        if(!found){
            LOG << "Warning, playlist altered, music was not found for UUID : " + line;
        }
    }
    playlist.close();
    save(filepath);
}
