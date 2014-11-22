#ifndef PLAYLISTGENERATOR_HPP
#define PLAYLISTGENERATOR_HPP

#include <map>
#include <vector>
#include <string>
#include "MusicFile.hpp"

class PlaylistGenerator
{
    public:

        PlaylistGenerator();
        void add(MusicFile * file);

    private:

        std::map<std::string, std::vector<MusicFile *> > dict;
};

#endif // PLAYLISTGENERATOR_HPP
