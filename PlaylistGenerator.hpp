#ifndef PLAYLISTGENERATOR_HPP
#define PLAYLISTGENERATOR_HPP

#include <vector>
#include <string>
#include "MusicFile.hpp"

class PlaylistGenerator
{
    public:

        PlaylistGenerator(std::string _basefolder);
        void add(MusicFile * file);
        void save(std::string filepath);

    private:

        std::string basefolder;
        std::vector<MusicFile *> musics;
};

#endif // PLAYLISTGENERATOR_HPP
