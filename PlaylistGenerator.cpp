#include "PlaylistGenerator.hpp"
#include "Utility.hpp"

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
    ofstream myfile(filepath.c_str(), ios::out | ios::trunc);
    for(vector<MusicFile *>::const_iterator i = musics.begin(); i != musics.end(); i++){
        string s = (*i)->getFilepath();
        s.erase(0, basefolder.size()+1);
        myfile << s << '\n';
    }
    myfile.close();
}
