#pragma once

#include <QAbstractTableModel>
#include "Playlist.hpp"

typedef std::pair<std::string, Musics> Album;
typedef std::vector<Album> AlbumVector;

class AlbumsModel : public QAbstractTableModel {
    Q_OBJECT
	public:

        enum {
            COLUMN_NAME,
            COLUMN_ARTISTS,
            COLUMN_GENRES,
            COLUMN_DURATION,
            COLUMN_RATING,
            COLUMN_KEYWORDS,
            COLUMN_SONG_COUNT,
            COLUMN_COUNT,
            COLUMN_MAX=COLUMN_COUNT-1
        };
        explicit AlbumsModel(QObject *parent = 0);
        virtual ~AlbumsModel();

        template<class Container>
        void set(const Container& _items) {
            beginResetModel();
            items.clear();
            for(const auto& _item : _items){
                items.push_back(_item);
            }
            endResetModel();
        }

        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
        const Album * itemAt(int row) const;

	private:

        QVariant infoAtColumn(const Album * ptr, int row) const;
        AlbumVector items;
};
