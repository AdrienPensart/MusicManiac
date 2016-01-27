#pragma once

#include <QAbstractTableModel>
#include "MusicFile.hpp"

class MusicModel : public QAbstractTableModel {
    Q_OBJECT
    public:
        enum {
            ROW_TITLE,
            ROW_ARTIST,
            ROW_ALBUM,
            ROW_RATING,
            ROW_KEYWORDS,
            ROW_GENRE,
            ROW_DURATION,
            ROW_FILEPATH,
            ROW_UUID,
            ROW_COUNT,
            ROW_MAX=ROW_COUNT-1
        };
        enum {
            COLUMN_KEY,
            COLUMN_VALUE,
            COLUMN_COUNT,
            COLUMN_MAX=COLUMN_COUNT-1
        };
        explicit MusicModel(QObject *parent = 0);
        virtual ~MusicModel();

        void set(MusicFile * mf);
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    private:

        MusicFile * mf;
};
