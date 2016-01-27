#include "AlbumModel.hpp"
#include "common/Utility.hpp"
#include <QDebug>

AlbumModel::AlbumModel(QObject *parent) :
    QAbstractTableModel(parent) {
}

AlbumModel::~AlbumModel() {
}

int AlbumModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : (int)items.size();
}

int AlbumModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

MusicFile * AlbumModel::itemAt(int row) const {
    if(row >= 0 && row < rowCount()) {
        return items.at(row);
    }
    return 0;
}

Qt::ItemFlags AlbumModel::flags(const QModelIndex &index) const {
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

QVariant AlbumModel::data(const QModelIndex& index, int role) const {
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
        auto item = itemAt(index.row());
        if(item) {
            return infoAtColumn(item, index.column());
        }
    }
    return QVariant();
}

bool AlbumModel::setData (const QModelIndex & index, const QVariant & value, int role) {
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

QVariant AlbumModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case COLUMN_TITLE:
                return tr("Title");
                break;
            case COLUMN_ARTIST:
                return tr("Artist");
                break;
            case COLUMN_GENRE:
                return tr("Genre");
                break;
            case COLUMN_ALBUM:
                return tr("Album");
                break;
            case COLUMN_RATING:
                return tr("Rating");
                break;
            case COLUMN_KEYWORDS:
                return tr("Tags");
                break;
            case COLUMN_UUID:
                return tr("UUID");
                break;
            default:
                return tr("Undefined column");
        }
    }
    return QVariant();
}

QVariant AlbumModel::infoAtColumn(MusicFile * item, int row) const {
    if(!item) {
        return tr("Undefined");
    }

    switch(row) {
        case COLUMN_TITLE:
            return item->getTitle().c_str();
            break;
        case COLUMN_ARTIST:
            return item->getArtist().c_str();
            break;
        case COLUMN_GENRE:
            return item->getGenre().c_str();
            break;
        case COLUMN_ALBUM:
            return item->getAlbum().c_str();
            break;
        case COLUMN_RATING:
            return item->getRating();
            break;
        case COLUMN_KEYWORDS:
            return item->getKeywords().c_str();
            break;
        case COLUMN_UUID:
            return item->getUUID().c_str();
            break;
        default:
            return tr("Undefined");
    }
    return tr("Undefined");
}
