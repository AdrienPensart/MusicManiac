#pragma once

#include <QAbstractTableModel>
#include "Playlist.hpp"

class PlaylistModel : public QAbstractTableModel {
	public:

		enum { COLUMN_FILEPATH, COLUMN_ARTISTS, COLUMN_GENRES, COLUMN_MIN_DURATION, COLUMN_MAX_DURATION, COLUMN_RATING, COLUMN_WITH_KEYWORDS, COLUMN_WITHOUT_KEYWORDS, COLUMN_COUNT, COLUMN_MAX=COLUMN_COUNT-1};
		explicit PlaylistModel(QObject *parent = 0);
		virtual ~PlaylistModel();

		int rowCount(const QModelIndex& parent) const;
		int columnCount(const QModelIndex& parent) const;
		QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
		Playlist * playlistAt(int row) const;
		const std::vector<Playlist*>& getPlaylists() const;
		void add(Playlist* mf);
		void clear();

	private:

		QVariant infoAtColumn(Playlist * ptr, int row) const;
		std::vector<Playlist*> playlists;

};
