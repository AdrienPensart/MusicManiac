#pragma once

#include <vector>
#include <string>
#include "MusicFile.hpp"

class PlaylistGenerator
{
    public:

        PlaylistGenerator(std::string _basefolder);
        void add(MusicFile * file);
        virtual void save(std::string filepath);

    private:

        std::string basefolder;
        std::vector<MusicFile *> musics;
};

class PlaylistRefresher : public PlaylistGenerator
{
    public:

        PlaylistRefresher(std::string basefolder, std::string uuid_playlist, const std::vector<MusicFile*>& musics);
        virtual void save(std::string filepath);

    private:

        std::string uuids;
        const std::vector<MusicFile*>& sources;
};
