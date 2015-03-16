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
        double progression();
        int getTotalCount();
        int getReadCount();

    private:

        unsigned int totalCount;
        unsigned int readCount;
        std::vector<std::string> playlists;
        QDir dir;
        QDirIterator iterator;
        bool regen;
};
