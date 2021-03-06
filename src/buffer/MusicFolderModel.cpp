#include "MusicFolderModel.hpp"
#include <common/Utility.hpp>
#include <QSet>

MusicFolderModel::MusicFolderModel(QObject *parent) :
	QAbstractTableModel(parent) {
}

MusicFolderModel::~MusicFolderModel() {
	clear();
}

QStringList MusicFolderModel::getKeywords() {
	QStringList keywords;
	for(std::vector<MusicFile*>::const_iterator ci = musics.begin(); ci != musics.end() ; ci++) {
		std::vector<std::string> currentKeywords;
		Common::split((*ci)->getKeywords(), " ", currentKeywords);
		for(std::vector<std::string>::const_iterator si = currentKeywords.begin(); si != currentKeywords.end(); si++) {
			keywords.append(QString::fromStdString(*si));
		}
	}
	QStringList sorted = keywords.toSet().toList();
	sorted.sort();
	return sorted;
}

QStringList MusicFolderModel::getArtists() {
	QStringList artists;
	for(std::vector<MusicFile*>::const_iterator ci = musics.begin(); ci != musics.end() ; ci++) {
		artists.append(QString::fromStdString((*ci)->getArtist()));
	}
	QStringList sorted = artists.toSet().toList();
	sorted.sort();
	return sorted;
}

QStringList MusicFolderModel::getGenres() {
	QStringList genres;
	for(std::vector<MusicFile*>::const_iterator ci = musics.begin(); ci != musics.end() ; ci++) {
		genres.append(QString::fromStdString((*ci)->getGenre()));
	}
	QStringList sorted = genres.toSet().toList();
	sorted.sort();
	return sorted;
}

const std::vector<MusicFile*>& MusicFolderModel::getMusics() const {
	return musics;
}

void MusicFolderModel::clear() {
	beginResetModel();
	// "Clearing music folder";
	for(std::vector<MusicFile*>::const_iterator ci = musics.begin(); ci != musics.end() ; ci++) {
		delete *ci;
	}
	musics.clear();
	endResetModel();
}

int MusicFolderModel::rowCount(const QModelIndex& /* parent */) const {
	return musics.size();
}

int MusicFolderModel::columnCount(const QModelIndex& /* parent */) const {
	return COLUMN_COUNT;
}

void MusicFolderModel::add(MusicFile*mf) {
	beginInsertRows(QModelIndex(), musics.size(), musics.size());
	musics.push_back(mf);
	endInsertRows();
}

MusicFile * MusicFolderModel::musicAt(int row) const {
	if(row >= 0 && row < (int)musics.size()) {
		return musics[row];
	}
	return 0;
}

Qt::ItemFlags MusicFolderModel::flags(const QModelIndex &index) const {
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
	}
    if(
            index.column() == COLUMN_KEYWORDS ||
            index.column() == COLUMN_RATING ||
            index.column() == COLUMN_ARTIST ||
            index.column() == COLUMN_ALBUM ||
            index.column() == COLUMN_GENRE) {
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	}
	return QAbstractItemModel::flags(index);// | Qt::ItemIsEditable;
}

QVariant MusicFolderModel::data(const QModelIndex& index, int role) const {
	if(!index.isValid()) {
		return QVariant();
	}

	if (index.row() >= (int)musics.size()) {
		return QVariant();
	}
	/*
	if(role == Qt::TextAlignmentRole) {
		return int(Qt::AlignHCenter | Qt::AlignVCenter);
	}
	*/

	if(role == Qt::DisplayRole || role == Qt::EditRole) {
		MusicFile * rowMusic = musicAt(index.row());
		if(rowMusic) {
			return infoAtColumn(rowMusic, index.column());
		}
	}
	return QVariant();
}

bool MusicFolderModel::setData (const QModelIndex & index, const QVariant & value, int role) {
	if (index.isValid() && role == Qt::EditRole) {
		// "Setting data : " + value.toString().toStdString();
		MusicFile * rowMusic = musicAt(index.row());
		if(rowMusic) {
			switch(index.column()) {
                case COLUMN_ARTIST:
                    rowMusic->setArtist(value.toString().toStdString());
                    break;
                case COLUMN_ALBUM:
                    rowMusic->setAlbum(value.toString().toStdString());
                    break;
                case COLUMN_GENRE:
                    rowMusic->setGenre(value.toString().toStdString());
                    break;
				case COLUMN_RATING:
					rowMusic->setRating(value.toDouble());
					break;
				case COLUMN_KEYWORDS:
					rowMusic->setKeywords(value.toString().toStdString());
					break;
			}
			emit dataChanged(index, index);
			return true;
		}
	}
	return false;
}

QVariant MusicFolderModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch(section) {
			case COLUMN_FILENAME:
				return tr("Filename");
				break;
			case COLUMN_ARTIST:
				return tr("Artist");
				break;
            case COLUMN_ALBUM:
                return tr("Album");
                break;
			case COLUMN_GENRE:
				return tr("Genre");
				break;
			case COLUMN_DURATION:
				return tr("Duration");
				break;
			case COLUMN_RATING:
				return tr("Rating");
				break;
			case COLUMN_KEYWORDS:
				return tr("Tags");
				break;
		}
	}
	return QVariant();
}

QVariant MusicFolderModel::infoAtColumn(MusicFile * mf, int row) const {
	if(!mf) {
		return "Undefined";
	}

	switch(row) {
		case COLUMN_FILENAME:
			return mf->getFilename().c_str();
			break;
		case COLUMN_ARTIST:
			return mf->getArtist().c_str();
			break;
        case COLUMN_ALBUM:
            return mf->getAlbum().c_str();
            break;
		case COLUMN_GENRE:
			return mf->getGenre().c_str();
			break;
		case COLUMN_DURATION:
			return mf->getDuration().c_str();
			break;
		case COLUMN_RATING:
			return mf->getRating();
			break;
		case COLUMN_KEYWORDS:
			return mf->getKeywords().c_str();
			break;
		default:
			return tr("Undefined");
	}
	return "Undefined";
}
