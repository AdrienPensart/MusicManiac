#pragma once

#include <QAbstractTableModel>
#include "common/Utility.hpp"
#include "MusicModel.hpp"

class AlbumModel : public QAbstractTableModel {
    Q_OBJECT
    public:

        enum {
            COLUMN_TITLE,
            COLUMN_ARTIST,
            COLUMN_ALBUM,
            COLUMN_GENRE,
            COLUMN_RATING,
            COLUMN_KEYWORDS,
            COLUMN_COUNT,
            COLUMN_MAX=COLUMN_COUNT-1
        };
        explicit AlbumModel(QObject *parent = 0);
        virtual ~AlbumModel();

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
        MusicFile * itemAt(int row) const;

    private:

        QVariant infoAtColumn(MusicFile * mf, int row) const;
        MusicVector items;
};
