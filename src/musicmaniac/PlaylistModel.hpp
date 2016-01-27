#pragma once

#include <QAbstractTableModel>
#include "Playlist.hpp"

template <typename M, typename V>
void MapToVec( const  M & m, V & v ) {
    v.clear();
    for( typename M::const_iterator it = m.begin(); it != m.end(); ++it ) {
        v.push_back( it->second );
    }
}

class PlaylistModel : public QAbstractTableModel {
    Q_OBJECT
	public:

        enum {
            COLUMN_TITLE,
            COLUMN_ARTIST,
            COLUMN_ALBUM,
            COLUMN_GENRE,
            COLUMN_RATING,
            COLUMN_KEYWORDS,
            COLUMN_UUID,
            COLUMN_COUNT,
            COLUMN_MAX=COLUMN_COUNT-1
        };
        explicit PlaylistModel(QObject *parent = 0);
		virtual ~PlaylistModel();

        void set(Playlist * _playlist);
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
        MusicFile * itemAt(int row) const;

	private:

        QVariant infoAtColumn(MusicFile * mf, int row) const;
        Playlist * playlist;
        MusicVector items;
};
