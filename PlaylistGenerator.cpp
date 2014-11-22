#include "PlaylistGenerator.hpp"
#include "Utility.hpp"

using namespace std;

PlaylistGenerator::PlaylistGenerator()
{
}

void PlaylistGenerator::add(MusicFile * file)
{
    vector<string> keywords;
    Common::split(file->getKeywords(), " ", keywords);
    for(vector<string>::iterator i = keywords.begin(); i != keywords.end(); i++){
        dict[*i].push_back(file);
    }
}
