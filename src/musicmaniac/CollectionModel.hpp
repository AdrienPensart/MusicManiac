#pragma once

#include <QStandardItemModel>

const QString GLOBAL_MANUAL_PLAYLISTS = "global_manual_playlists";
const QString GLOBAL_AUTO_PLAYLISTS = "global_auto_playlists";
const QString ARTIST_MANUAL_PLAYLISTS = "artist_manual_playlists";
const QString ARTIST_AUTO_PLAYLISTS = "artist_auto_playlists";

const QString ARTIST = "artist";
const QString ALBUM = "album";
const QString ALBUMS = "albums";
const QString SONG = "song";
const QString SONGS = "songs";
const QString PLAYLIST = "playlist";

enum CollectionRoles {
    ItemTypeRole = Qt::UserRole + 1,
    ItemKey
};

class CollectionModel : public QStandardItemModel {
    Q_OBJECT
    public:
        explicit CollectionModel(QObject *parent = 0);
        virtual ~CollectionModel();
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
        virtual QStringList mimeTypes() const;
};

