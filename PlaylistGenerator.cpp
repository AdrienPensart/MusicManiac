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
    return imploded.str();
}

PlaylistGenerator::PlaylistGenerator() {
}

void PlaylistGenerator::add(MusicFile * music) {
    musics.push_back(music);
}

void PlaylistGenerator::save(std::string filepath){
    // m3u playlist
    string m3u = filepath+".m3u";
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
        m3u_file << "#EXTREM: " << (*i)->getUUID() << '\n';
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

PlaylistRefresher::PlaylistRefresher(string _uuids, const std::vector<MusicFile*>& _sources)
    : uuids(_uuids), sources(_sources){
}

void PlaylistRefresher::save(std::string filepath){
    ifstream uuids_file(uuids.c_str());
    std::string line;
    while(std::getline(uuids_file, line)){
        bool found = false;
        for(std::vector<MusicFile *>::const_iterator i = sources.begin(); i != sources.end(); i++){
            string uuid = (*i)->getUUID();
            if(uuid == line){
                add(*i);
                found = true;
                break;
            }
        }
        if(!found){
            LOG << "Warning, playlist altered, music was not found for UUID : " + line;
        }
    }
    uuids_file.close();
    PlaylistGenerator::save(filepath);
}
