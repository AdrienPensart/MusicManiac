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
    return parent.isValid() ? 0 : ROW_COUNT;
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
        switch(index.row()){
            case ROW_TITLE:
                return index.column() == 0 ? tr("Title") : mf->getTitle().c_str();
                break;
            case ROW_FILEPATH:
                return index.column() == 0 ? tr("Filepath") : mf->getFilepath().c_str();
                break;
            case ROW_ARTIST:
                return index.column() == 0 ? tr("Artist") : mf->getArtist().c_str();
                break;
            case ROW_ALBUM:
                return index.column() == 0 ? tr("Album") : mf->getAlbum().c_str();
                break;
            case ROW_UUID:
                return index.column() == 0 ? tr("UUID") : mf->getUUID().c_str();
                break;
            case ROW_RATING:
                return index.column() == 0 ? tr("Rating") : QString::number(mf->getRating());
                break;
            case ROW_KEYWORDS:
                return index.column() == 0 ? tr("Keywords") : mf->getKeywords().c_str();
                break;
            case ROW_GENRE:
                return index.column() == 0 ? tr("Genre") : mf->getGenre().c_str();
                break;
            case ROW_DURATION:
                return index.column() == 0 ? tr("Duration") : mf->getDuration().c_str();
                break;
            default:
                return tr("Undefined");
        }
    }
    return tr("Undefined");
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
            case COLUMN_KEY:
                return tr("Key");
                break;
            case COLUMN_VALUE:
                return tr("Value");
                break;
            default:
                return tr("Undefined column");
        }
    }
    return QVariant();
}
