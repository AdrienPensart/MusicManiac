#ifndef MUSICFOLDERMODEL_HPP
#define MUSICFOLDERMODEL_HPP

#include <QAbstractTableModel>
#include <QVector>

#include "MusicFile.hpp"

class MusicFolderModel : public QAbstractTableModel
{
    Q_OBJECT

    public:

        explicit MusicFolderModel(QObject *parent = 0);
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data (const QModelIndex& index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

        void add(MusicFile*mf);
        void clear();

    private:

        MusicFile * musicAt(int offset) const;
        QString infoAtColumn(MusicFile * ptr, int offset) const;

        QVector<MusicFile*> music;
};

#endif // MUSICFOLDERMODEL_HPP
