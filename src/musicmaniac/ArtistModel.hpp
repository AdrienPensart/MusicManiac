#pragma once

#include <QAbstractTableModel>
#include "Playlist.hpp"

class ArtistModel : public QAbstractTableModel {
    Q_OBJECT
    public:
        enum {
            COLUMN_NAME,
            COLUMN_MUSICS_COUNT,
            COLUMN_ALBUMS_COUNT,
            COLUMN_PLAYLISTS_COUNT,
            COLUMN_AVR_RATING,
            COLUMN_KEYWORDS,
            COLUMN_GENRES,
            COLUMN_DURATION,
            COLUMN_COUNT,
            COLUMN_MAX=COLUMN_COUNT-1
        };
        explicit ArtistModel(QObject *parent = 0);
        virtual ~ArtistModel();

        void set(QString _artist, const Musics& musics, const MusicsByAlbums& albums, const Playlists& playlists);
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    private:

        QString artist;
        QString keywords;
        QString genres;
        QString duration;
        int musicsCount;
        int albumsCount;
        int playlistsCount;
        double averageRating;
};
