#include "PlaylistsModel.hpp"
#include "common/Utility.hpp"
#include <QDebug>

PlaylistsModel::PlaylistsModel(QObject *parent) :
    QAbstractTableModel(parent) {
}

PlaylistsModel::~PlaylistsModel() {
}

int PlaylistsModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : (int)items.size();
}

int PlaylistsModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

Playlist * PlaylistsModel::itemAt(int row) const {
    if(row >= 0 && row < rowCount()) {
        return items[row];
	}
	return 0;
}

Qt::ItemFlags PlaylistsModel::flags(const QModelIndex &index) const {
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

QVariant PlaylistsModel::data(const QModelIndex& index, int role) const {
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

bool PlaylistsModel::setData (const QModelIndex & index, const QVariant & value, int role) {
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

QVariant PlaylistsModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
            case COLUMN_DURATION:
                return tr("Duration");
                break;
			case COLUMN_MIN_DURATION:
				return tr("Min dur.");
				break;
			case COLUMN_MAX_DURATION:
				return tr("Max dur.");
				break;
            case COLUMN_AVR_RATING:
                return tr("Avr. Rating");
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

QVariant PlaylistsModel::infoAtColumn(Playlist * item, int column) const {
    if(!item) {
        return QVariant();
	}

    switch(column) {
		case COLUMN_FILEPATH:
            return item->getFilename().c_str();
			break;
		case COLUMN_ARTISTS:
            return Common::implode(item->getArtists()).c_str();
			break;
        case COLUMN_DURATION:
            return item->getDuration().c_str();
            break;
		case COLUMN_GENRES:
            return Common::implode(item->getGenres()).c_str();
			break;
        case COLUMN_MIN_DURATION:
            return item->getMinDuration().c_str();
			break;
		case COLUMN_MAX_DURATION:
            return item->getMaxDuration().c_str();
			break;
		case COLUMN_RATING:
            return item->getRating();
			break;
        case COLUMN_AVR_RATING:
            return item->getAverageRating();
            break;
		case COLUMN_WITH_KEYWORDS:
            return Common::implode(item->getWith()).c_str();
			break;
		case COLUMN_WITHOUT_KEYWORDS:
            return Common::implode(item->getWithout()).c_str();
			break;
		default:
			return tr("Undefined");
	}
    return QVariant();
}
