#pragma once

#include <vector>
#include <string>
#include <QDirIterator>
#include "MusicFile.hpp"

class MusicFileFactory
{
    public:

        MusicFileFactory(QString folder, bool regen=false);
        MusicFile * factory();
        const std::vector<std::string>& getPlaylists();
        bool valid();

    private:

        std::vector<std::string> playlists;
        QDir music;
        QDirIterator iterator;
        bool regen;
};
