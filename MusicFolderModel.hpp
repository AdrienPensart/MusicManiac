#ifndef MUSICFOLDERMODEL_HPP
#define MUSICFOLDERMODEL_HPP

#include <QAbstractTableModel>
#include <QVector>

#include "MusicFile.hpp"

class MusicFolderModel : public QAbstractTableModel
{
    Q_OBJECT

    public:

        enum { COLUMN_PATH, COLUMN_UUID, COLUMN_DURATION, COLUMN_RATING, COLUMN_KEYWORDS, COLUMN_COUNT, COLUMN_MAX=COLUMN_COUNT-1};
        explicit MusicFolderModel(QObject *parent = 0);
        virtual ~MusicFolderModel();

        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

        void add(MusicFile*mf);
        void clear();
        QStringList getKeywords();

    private:

        MusicFile * musicAt(int offset) const;
        QVariant infoAtColumn(MusicFile * ptr, int offset) const;

        QVector<MusicFile*> music;
};

#endif // MUSICFOLDERMODEL_HPP
