#pragma once

#include <QDirIterator>
#include "MusicFile.hpp"

class MusicFileFactory
{
    public:

        MusicFileFactory(QString folder, bool regen=false);
        MusicFile * factory();
        bool valid();

    private:

        QDir music;
        QDirIterator iterator;
        bool regen;
};
