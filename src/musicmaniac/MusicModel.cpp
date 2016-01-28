#include "MusicModel.hpp"
#include "common/Utility.hpp"
#include <set>
#include <QDebug>

MusicModel::MusicModel(QObject *parent) :
    QAbstractTableModel(parent), mf(nullptr) {
}

MusicModel::~MusicModel() {
}

void MusicModel::set(MusicFile * _mf) {
    beginResetModel();
    mf = _mf;
    endResetModel();
}

int MusicModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : 1;
}

int MusicModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

Qt::ItemFlags MusicModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    /*
    if(index.column() == COLUMN_KEYWORDS || index.column() == COLUMN_RATING) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    */
    return QAbstractItemModel::flags(index);// | Qt::ItemIsEditable;
}

QVariant MusicModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= rowCount()) {
        return QVariant();
    }

    if(role == Qt::TextAlignmentRole) {
        return int(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column()){
            case COLUMN_TITLE:
                return mf->getTitle().c_str();
                break;
            case COLUMN_FILEPATH:
                return mf->getFilepath().c_str();
                break;
            case COLUMN_ARTIST:
                return mf->getArtist().c_str();
                break;
            case COLUMN_ALBUM:
                return mf->getAlbum().c_str();
                break;
            case COLUMN_UUID:
                return mf->getUUID().c_str();
                break;
            case COLUMN_RATING:
                return QString::number(mf->getRating());
                break;
            case COLUMN_KEYWORDS:
                return mf->getKeywords().c_str();
                break;
            case COLUMN_GENRE:
                return mf->getGenre().c_str();
                break;
            case COLUMN_DURATION:
                return mf->getDuration().c_str();
                break;
            default:
                return tr("Undefined");
        }
    }
    return QVariant();
}

bool MusicModel::setData (const QModelIndex & index, const QVariant & value, int role) {
    /*
    if (index.isValid() && role == Qt::EditRole) {
        // "Setting data : " + value.toString().toStdString();
        Playlist * rowMusic = musicAt(index.row());
        if(rowMusic) {
            switch(index.column()) {
                case COLUMN_RATING:
                    rowMusic->setRating(value.toDouble());
                    break;
                case COLUMN_KEYWORDS:
                    rowMusic->setKeywords(value.toString().toStdString());
                    break;
                case COLUMN_GENRE:
                    rowMusic->setGenre(value.toString().toStdString());
                    break;
            }
            emit dataChanged(index, index);
            return true;
        }
    }
    */
    return false;
}

QVariant MusicModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case COLUMN_TITLE:
                return tr("Title");
                break;
            case COLUMN_FILEPATH:
                return tr("Filepath");
                break;
            case COLUMN_ARTIST:
                return tr("Artist");
                break;
            case COLUMN_ALBUM:
                return tr("Album");
                break;
            case COLUMN_UUID:
                return tr("UUID");
                break;
            case COLUMN_RATING:
                return tr("Rating");
                break;
            case COLUMN_KEYWORDS:
                return tr("Keywords");
                break;
            case COLUMN_GENRE:
                return tr("Genre");
                break;
            case COLUMN_DURATION:
                return tr("Duration");
                break;
            default:
                return tr("Undefined");
        }
    }
    return QVariant();
}
