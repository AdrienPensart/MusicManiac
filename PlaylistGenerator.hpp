#pragma once

#include <vector>
#include <string>
#include "MusicFile.hpp"

class PlaylistGenerator
{
    public:

        PlaylistGenerator();
        void add(MusicFile * file);
        virtual void save(std::string filepath);
        void setBasefolder(std::string _basefolder);
        void setRating(const std::string& _rating);
        void setMaxDuration(const std::string& _maxDuration);
        void setMinDuration(const std::string& _minDuration);
        void setWith(const std::vector<std::string>& _with);
        void setWithout(const std::vector<std::string>& _without);

    private:

        std::string rating;
        std::string minDuration;
        std::string maxDuration;
        std::vector<std::string> without;
        std::vector<std::string> with;
        std::string basefolder;
        std::vector<MusicFile *> musics;
};

class PlaylistRefresher : public PlaylistGenerator
{
    public:

        PlaylistRefresher(std::string uuid_playlist, const std::vector<MusicFile*>& musics);
        virtual void save(std::string filepath);

    private:

        std::string uuids;
        const std::vector<MusicFile*>& sources;
};
