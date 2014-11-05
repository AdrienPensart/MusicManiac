#ifndef PLAYLISTGENERATOR_HPP
#define PLAYLISTGENERATOR_HPP

#include <QMap>
#include <QList>
#include "MusicFile.hpp"

class PlaylistGenerator
{
    public:

        PlaylistGenerator();
        void add(MusicFile * file);

    private:

        QMap<QString, QList<MusicFile *> > dict;
};

#endif // PLAYLISTGENERATOR_HPP
