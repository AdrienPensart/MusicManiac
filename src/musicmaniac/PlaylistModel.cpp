#include "PlaylistModel.hpp"
#include "common/Utility.hpp"
#include <QDebug>

PlaylistModel::PlaylistModel(QObject *parent) :
    QAbstractTableModel(parent) {
}

PlaylistModel::~PlaylistModel() {
}

int PlaylistModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : (int)playlists.size();
}

int PlaylistModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

Playlist * PlaylistModel::playlistAt(int row) const {
    qDebug() << "This = " << this;
    qDebug() << "PlaylistAt : " << row << " with playlists size : " << rowCount();
    qDebug() << "At address : " << &playlists;
    if(row >= 0 && row < rowCount()) {
        return playlists[row];
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
    qDebug() << "This = " << this;
    qDebug() << "Playlists size : " << rowCount();
    qDebug() << "At address : " << &playlists;
	if(!index.isValid()) {
        qDebug() << "Index invalide";
		return QVariant();
	}

    if (index.row() >= rowCount()) {
        qDebug() << "Row invalide : " << index.row();
		return QVariant();
	}

	if(role == Qt::TextAlignmentRole) {
		return int(Qt::AlignHCenter | Qt::AlignVCenter);
	}

	if(role == Qt::DisplayRole || role == Qt::EditRole) {
		Playlist * rowPlaylist = playlistAt(index.row());
		if(rowPlaylist) {
            auto info = infoAtColumn(rowPlaylist, index.column());
            qDebug() << "info : " << info.toString();
            return info;
        } else {
            qDebug() << "Bad playlist";
        }
	}
    qDebug() << "Bad role : " << role;
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
			case COLUMN_FILEPATH:
				return tr("File");
				break;
			case COLUMN_ARTISTS:
				return tr("Artists");
				break;
			case COLUMN_GENRES:
				return tr("Genres");
				break;
			case COLUMN_MIN_DURATION:
				return tr("Min dur.");
				break;
			case COLUMN_MAX_DURATION:
				return tr("Max dur.");
				break;
			case COLUMN_RATING:
				return tr("Rating");
				break;
			case COLUMN_WITH_KEYWORDS:
				return tr("W/ Tags");
				break;
			case COLUMN_WITHOUT_KEYWORDS:
				return tr("W/O Tags");
				break;
		}
	}
	return QVariant();
}

QVariant PlaylistModel::infoAtColumn(Playlist * pl, int row) const {
	if(!pl) {
        return tr("Undefined");
	}

	switch(row) {
		case COLUMN_FILEPATH:
            qDebug() << pl->getFilename().c_str();
            return pl->getFilename().c_str();
			break;
		case COLUMN_ARTISTS:
            qDebug() << Common::implode(pl->getArtists()).c_str();
			return Common::implode(pl->getArtists()).c_str();
			break;
		case COLUMN_GENRES:
            qDebug() << Common::implode(pl->getGenres()).c_str();
			return Common::implode(pl->getGenres()).c_str();
			break;
        case COLUMN_MIN_DURATION:
            qDebug() << pl->getMinDuration().c_str();
			return pl->getMinDuration().c_str();
			break;
		case COLUMN_MAX_DURATION:
            qDebug() << pl->getMaxDuration().c_str();
			return pl->getMaxDuration().c_str();
			break;
		case COLUMN_RATING:
            qDebug() << pl->getRating();
			return pl->getRating();
			break;
		case COLUMN_WITH_KEYWORDS:
            qDebug() << Common::implode(pl->getWith()).c_str();
			return Common::implode(pl->getWith()).c_str();
			break;
		case COLUMN_WITHOUT_KEYWORDS:
            qDebug() << Common::implode(pl->getWithout()).c_str();
			return Common::implode(pl->getWithout()).c_str();
			break;
		default:
			return tr("Undefined");
	}
    return tr("Undefined");
}
