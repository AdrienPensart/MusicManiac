#include "AlbumsModel.hpp"
#include "common/Utility.hpp"
#include <QDebug>
#include <set>

AlbumsModel::AlbumsModel(QObject *parent) :
    QAbstractTableModel(parent) {
}

AlbumsModel::~AlbumsModel() {
}

int AlbumsModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : (int)items.size();
}

int AlbumsModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

const Album * AlbumsModel::itemAt(int row) const {
    if(row >= 0 && row < rowCount()) {
        return &(items.at(row));
	}
	return 0;
}

Qt::ItemFlags AlbumsModel::flags(const QModelIndex &index) const {
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

QVariant AlbumsModel::data(const QModelIndex& index, int role) const {
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

bool AlbumsModel::setData (const QModelIndex & index, const QVariant & value, int role) {
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

QVariant AlbumsModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch(section) {
            case COLUMN_NAME:
                return tr("Name");
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
			case COLUMN_RATING:
				return tr("Rating");
				break;
            case COLUMN_KEYWORDS:
                return tr("Keywords");
				break;
            case COLUMN_SONG_COUNT:
                return tr("Songs");
				break;
            default:
                return tr("Undefined column");
		}
	}
	return QVariant();
}

QVariant AlbumsModel::infoAtColumn(const Album * item, int column) const {
    if(!item) {
        return QVariant();
	}

    switch(column) {
        case COLUMN_NAME:
            return item->first.c_str();
			break;
		case COLUMN_ARTISTS:
            {
                std::set<std::string> artists;
                for(const auto& song : item->second){
                    artists.insert(song.second->getArtist());
                }
                return Common::implode(artists).c_str();
            }
			break;
        case COLUMN_GENRES:
            {
                std::set<std::string> genres;
                for(const auto& song : item->second){
                    genres.insert(song.second->getGenre());
                }
                return Common::implode(genres).c_str();
            }
            break;
        case COLUMN_DURATION:
            {
                int totalSeconds = 0;
                for(const auto& song : item->second){
                    totalSeconds += song.second->getDurationInSeconds();
                }
                return secondsToString(totalSeconds).c_str();
            }
			break;
		case COLUMN_RATING:
            {
                double totalRating = 0.0;
                for(const auto& song : item->second){
                    totalRating += song.second->getRating();
                }
                totalRating /= (double)item->second.size();
                return totalRating;
            }
			break;
        case COLUMN_KEYWORDS:
            {
                std::set<std::string> keywords;
                for(const auto& song : item->second){
                    auto songKeywords = song.second->getSplittedKeywords();
                    keywords.insert(songKeywords.begin(), songKeywords.end());
                }
                return Common::implode(keywords).c_str();
            }
			break;
        case COLUMN_SONG_COUNT:
            return (int)item->second.size();
			break;
		default:
			return tr("Undefined");
	}
    return QVariant();
}
