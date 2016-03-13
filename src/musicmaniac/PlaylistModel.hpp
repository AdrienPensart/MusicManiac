#pragma once

#include <QAbstractTableModel>

class Playlist;
class Collection;
class MusicFile;

class PlaylistModel : public QAbstractTableModel {
    Q_OBJECT
	public:

        enum {
            COLUMN_TITLE,
            COLUMN_ARTIST,
            COLUMN_ALBUM,
            COLUMN_GENRE,
            COLUMN_DURATION,
            COLUMN_RATING,
            COLUMN_KEYWORDS,
            COLUMN_UUID,
            COLUMN_COUNT,
            COLUMN_MAX=COLUMN_COUNT-1
        };
        explicit PlaylistModel(Collection& collection, QObject *parent = 0);
		virtual ~PlaylistModel();

        Playlist * getPlaylist();
        void set(Playlist * _playlist);
        void clear();
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
        MusicFile * itemAt(int row) const;
        virtual Qt::DropActions supportedDropActions() const;
        virtual Qt::DropActions supportedDragActions() const;
        virtual bool dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent);
        virtual QStringList mimeTypes() const;
        virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

	private:

        QVariant infoAtColumn(MusicFile * mf, int row) const;
        Playlist * playlist;
        Collection& collection;
};
