#include "PlaylistGenerator.hpp"

PlaylistGenerator::PlaylistGenerator()
{
}

void PlaylistGenerator::add(MusicFile * file)
{
    QStringList keywords = file->getKeywords().split(" ");
    for(QStringList::iterator i = keywords.begin(); i != keywords.end(); i++){
        dict[*i].append(file);
    }
}
