#include "PlaylistModel.hpp"
#include "common/Utility.hpp"
#include <QDebug>

PlaylistModel::PlaylistModel(QObject *parent) :
    QAbstractTableModel(parent) {
}

PlaylistModel::~PlaylistModel() {
}

int PlaylistModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : (int)mv.size();
}

int PlaylistModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

MusicFile * PlaylistModel::songAt(int row) const {
    if(row >= 0 && row < rowCount()) {
        return mv.at(row);
	}
	return 0;
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const {
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

QVariant PlaylistModel::data(const QModelIndex& index, int role) const {
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
        auto rowPlaylist = songAt(index.row());
		if(rowPlaylist) {
            return infoAtColumn(rowPlaylist, index.column());
        }
	}
	return QVariant();
}

bool PlaylistModel::setData (const QModelIndex & index, const QVariant & value, int role) {
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

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
		}
	}
	return QVariant();
}

QVariant PlaylistModel::infoAtColumn(MusicFile * mf, int row) const {
    if(!mf) {
        return tr("Undefined");
	}

	switch(row) {
        case COLUMN_TITLE:
            return mf->getTitle().c_str();
			break;
        case COLUMN_ARTIST:
            return mf->getArtist().c_str();
			break;
        case COLUMN_GENRE:
            return mf->getGenre().c_str();
			break;
        case COLUMN_ALBUM:
            return mf->getAlbum().c_str();
			break;
		case COLUMN_RATING:
            return mf->getRating();
			break;
        case COLUMN_KEYWORDS:
            return mf->getKeywords().c_str();
			break;
        case COLUMN_UUID:
            return mf->getUUID().c_str();
			break;
		default:
			return tr("Undefined");
	}
    return tr("Undefined");
}
