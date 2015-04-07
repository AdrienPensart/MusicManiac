#include "PlaylistModel.hpp"
#include "common/Utility.hpp"

PlaylistModel::PlaylistModel(QObject *parent) :
	QAbstractTableModel(parent) {
}

PlaylistModel::~PlaylistModel() {
	clear();
}

const std::vector<Playlist*>& PlaylistModel::getPlaylists() const {
	return playlists;
}

void PlaylistModel::clear() {
	beginResetModel();
	for(std::vector<Playlist*>::const_iterator ci = playlists.begin(); ci != playlists.end() ; ci++) {
		delete *ci;
	}
	playlists.clear();
	endResetModel();
}

int PlaylistModel::rowCount(const QModelIndex& /* parent */) const {
	return playlists.size();
}

int PlaylistModel::columnCount(const QModelIndex& /* parent */) const {
	return COLUMN_COUNT;
}

void PlaylistModel::add(Playlist* pl) {
	beginInsertRows(QModelIndex(), playlists.size(), playlists.size());
	playlists.push_back(pl);
	endInsertRows();
}

Playlist * PlaylistModel::playlistAt(int row) const {
	if(row >= 0 && row < (int)playlists.size()) {
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
	if(!index.isValid()) {
		return QVariant();
	}

	if (index.row() >= (int)playlists.size()) {
		return QVariant();
	}
	/*
	if(role == Qt::TextAlignmentRole) {
		return int(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	*/

	if(role == Qt::DisplayRole || role == Qt::EditRole) {
		Playlist * rowPlaylist = playlistAt(index.row());
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
			case COLUMN_FILEPATH:
				return tr("Filepath");
				break;
			case COLUMN_ARTISTS:
				return tr("Artists");
				break;
			case COLUMN_GENRES:
				return tr("Genres");
				break;
			case COLUMN_MIN_DURATION:
				return tr("Min duration");
				break;
			case COLUMN_MAX_DURATION:
				return tr("Max duration");
				break;
			case COLUMN_RATING:
				return tr("Rating");
				break;
			case COLUMN_WITH_KEYWORDS:
				return tr("With keywords");
				break;
			case COLUMN_WITHOUT_KEYWORDS:
				return tr("Without keywords");
				break;
		}
	}
	return QVariant();
}

QVariant PlaylistModel::infoAtColumn(Playlist * pl, int row) const {
	if(!pl) {
		return "Undefined";
	}

	switch(row) {
		case COLUMN_FILEPATH:
			return pl->getFilepath().c_str();
			break;
		case COLUMN_ARTISTS:
			return Common::implode(pl->getArtists()).c_str();
			break;
		case COLUMN_GENRES:
			return Common::implode(pl->getGenres()).c_str();
			break;
		case COLUMN_MIN_DURATION:
			return pl->getMinDuration().c_str();
			break;
		case COLUMN_MAX_DURATION:
			return pl->getMaxDuration().c_str();
			break;
		case COLUMN_RATING:
			return pl->getRating();
			break;
		case COLUMN_WITH_KEYWORDS:
			return Common::implode(pl->getWith()).c_str();
			break;
		case COLUMN_WITHOUT_KEYWORDS:
			return Common::implode(pl->getWithout()).c_str();
			break;
		default:
			return tr("Undefined");
	}
	return "Undefined";
}
