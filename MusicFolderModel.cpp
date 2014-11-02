#include "MusicFolderModel.hpp"

enum { COLUMN_PATH, COLUMN_UUID, COLUMN_DURATION, COLUMN_RATING, COLUMN_KEYWORDS, COLUMN_COUNT, COLUMN_MAX=COLUMN_COUNT-1};

MusicFolderModel::MusicFolderModel(QObject *parent) :
    QAbstractTableModel(parent) {
}

void MusicFolderModel::clear(){

}

int MusicFolderModel::rowCount(const QModelIndex& /* parent */) const {
    return music.size();
}

int MusicFolderModel::columnCount(const QModelIndex& /* parent */) const {
    return COLUMN_COUNT;
}

void MusicFolderModel::add(MusicFile*mf) {
    beginInsertRows(QModelIndex(), music.size(), music.size());
    music.append(mf);
    endInsertRows();
}

MusicFile * MusicFolderModel::musicAt(int offset) const {
    if(offset >= 0 && offset < (int)music.size())
    {
        return music[offset];
    }
    return 0;
}

QVariant MusicFolderModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid())
    {
        return QVariant();
    }
    else if(role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    else if(role == Qt::DisplayRole)
    {
        MusicFile * rowMusic = musicAt(index.row());
        if(rowMusic != 0)
        {
            return infoAtColumn(rowMusic, index.column());
        }
    }
    return QVariant();
}

QVariant MusicFolderModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case COLUMN_PATH:
                return tr("Path");
                break;
            case COLUMN_UUID:
                return tr("UUID");
                break;
            case COLUMN_DURATION:
                return tr("Duration");
                break;
            case COLUMN_RATING:
                return tr("Rating");
                break;
            case COLUMN_KEYWORDS:
                return tr("Keywords");
                break;
        }
    }
    return QVariant();
}

QString MusicFolderModel::infoAtColumn(MusicFile * mf, int offset) const
{
    if(!mf){
        return "Undefined";
    }

    switch(offset){
        case COLUMN_PATH:
            return mf->getFilepath();
            break;
        case COLUMN_UUID:
            return mf->getUUID();
            break;
        case COLUMN_DURATION:
            return mf->getDuration();
            break;
        case COLUMN_RATING:
            return QString::number(mf->getRating());
            break;
        case COLUMN_KEYWORDS:
            return mf->getKeywords().join(" ");
            break;
        default:
            return tr("Undefined");
    }
    return "Undefined";
}
