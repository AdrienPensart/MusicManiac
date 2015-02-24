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
        void refresh(std::string filepath, const std::vector<MusicFile *>& sources);
        void validate(std::string filepath, const std::vector<MusicFile *>& sources);

    private:

        std::string rating;
        std::string minDuration;
        std::string maxDuration;
        std::vector<std::string> without;
        std::vector<std::string> with;
        std::string basefolder;
        std::vector<MusicFile *> musics;
};
