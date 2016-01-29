#pragma once

#include <QAbstractTableModel>
#include "Playlist.hpp"

class PlaylistsModel : public QAbstractTableModel {
    Q_OBJECT
	public:

        enum {
            COLUMN_FILEPATH,
            COLUMN_ARTISTS,
            COLUMN_GENRES,
            COLUMN_DURATION,
            COLUMN_MIN_DURATION,
            COLUMN_MAX_DURATION,
            COLUMN_AVR_RATING,
            COLUMN_RATING,
            COLUMN_WITH_KEYWORDS,
            COLUMN_WITHOUT_KEYWORDS,
            COLUMN_AUTOGEN,
            COLUMN_COUNT,
            COLUMN_MAX=COLUMN_COUNT-1
        };
        explicit PlaylistsModel(QObject *parent = 0);
        virtual ~PlaylistsModel();

        template<class Container>
        void set(const Container& _items) {
            beginResetModel();
            Common::MapToVec(_items, items);
            endResetModel();
        }

        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
        Playlist * itemAt(int row) const;

	private:

		QVariant infoAtColumn(Playlist * ptr, int row) const;
        std::vector<Playlist*> items;
};
