#include "PlaylistGenerator.hpp"
#include <common/Utility.hpp>

#include <fstream>
using namespace std;

PlaylistGenerator::PlaylistGenerator(string _basefolder)
    : basefolder(_basefolder)
{
}

void PlaylistGenerator::add(MusicFile * music)
{
    musics.push_back(music);
}

void PlaylistGenerator::save(std::string filepath){
    // uuid playlist
    string music = filepath+".music";
    ofstream music_file(music.c_str(), ios::out | ios::trunc);
    for(vector<MusicFile *>::const_iterator i = musics.begin(); i != musics.end(); i++){
        string s = (*i)->getUUID();
        music_file << s << '\n';
    }
    music_file.close();

    // m3u playlist
    string m3u = filepath+".m3u";
    ofstream m3u_file(m3u.c_str(), ios::out | ios::trunc);
    m3u_file << "#EXTM3U\n";
    for(vector<MusicFile *>::const_iterator i = musics.begin(); i != musics.end(); i++){
        string s = (*i)->getFilepath();
        s.erase(0, basefolder.size()+1);
        m3u_file << s << '\n';
    }
    m3u_file.close();
}
