#pragma once

#include <QAbstractTableModel>
#include "Playlist.hpp"

class PlaylistModel : public QAbstractTableModel {
    Q_OBJECT
	public:

		enum { COLUMN_FILEPATH, COLUMN_ARTISTS, COLUMN_GENRES, COLUMN_MIN_DURATION, COLUMN_MAX_DURATION, COLUMN_RATING, COLUMN_WITH_KEYWORDS, COLUMN_WITHOUT_KEYWORDS, COLUMN_COUNT, COLUMN_MAX=COLUMN_COUNT-1};
        explicit PlaylistModel(QObject *parent = 0);
		virtual ~PlaylistModel();

        template<class Container>
        void set(const Container& _playlists) {
            beginResetModel();
            playlists.clear();
            playlists.insert(playlists.begin(), _playlists.begin(), _playlists.end());
            endResetModel();
        }

        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
		Playlist * playlistAt(int row) const;

	private:

		QVariant infoAtColumn(Playlist * ptr, int row) const;
        PlaylistVector playlists;
};
