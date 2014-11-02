#ifndef MUSICFILEFACTORY_H
#define MUSICFILEFACTORY_H

#include <QDirIterator>
#include "MusicFile.hpp"

class MusicFileFactory
{
    public:

        MusicFileFactory(QString folder);
        MusicFile * factory();
        bool valid();

    private:

        QDir music;
        QDirIterator iterator;
};

#endif // MUSICFILEFACTORY_H
