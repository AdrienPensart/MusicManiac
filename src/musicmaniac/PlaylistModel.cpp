#include "PlaylistModel.hpp"
#include "DragAndDropMusic.hpp"
#include "Playlist.hpp"
#include "Collection.hpp"
#include "common/Utility.hpp"
#include <QDebug>

PlaylistModel::PlaylistModel(Collection& _collection, QObject *parent) :
    QAbstractTableModel(parent), collection(_collection), playlist(nullptr) {
}

PlaylistModel::~PlaylistModel() {
}

void PlaylistModel::set(Playlist * _playlist) {
    beginResetModel();
    playlist = _playlist;
    endResetModel();
}

void PlaylistModel::clear(){
    beginResetModel();
    if(playlist){
        playlist->getMusics().clear();
    }
    endResetModel();
}

Playlist * PlaylistModel::getPlaylist(){
    return playlist;
}

int PlaylistModel::rowCount(const QModelIndex& parent) const {
    if(playlist){
        return parent.isValid() ? 0 : (int)playlist->getMusics().size();
    }
    return 0;
}

int PlaylistModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

MusicFile * PlaylistModel::itemAt(int row) const {
    if(playlist && row >= 0 && row < rowCount()) {
        return playlist->getMusics().at(row);
	}
	return 0;
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if(playlist && !playlist->isManual()){
        return defaultFlags;
    }

    if(index.isValid()){
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | defaultFlags;
    }
    return Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | defaultFlags;
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
        auto item = itemAt(index.row());
        if(item) {
            return infoAtColumn(item, index.column());
        }
	}
	return QVariant();
}

bool PlaylistModel::setData (const QModelIndex & index, const QVariant & value, int role) {
    if(index.isValid()){
        qDebug() << "setData" << index;
    }
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
            case COLUMN_DURATION:
                return tr("Duration");
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

QVariant PlaylistModel::infoAtColumn(MusicFile * item, int column) const {
    if(!item) {
        return QVariant();
	}

    switch(column) {
        case COLUMN_TITLE:
            return item->getTitle().c_str();
			break;
        case COLUMN_ARTIST:
            return item->getArtist().c_str();
			break;
        case COLUMN_DURATION:
            return item->getDuration().c_str();
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
    return QVariant();
}

QStringList PlaylistModel::mimeTypes() const {
    QStringList types;
    types << MIME_TYPE;
    return types;
}

bool PlaylistModel::dropMimeData(const QMimeData * mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent){
    QByteArray encodedData = mimeData->data(MIME_TYPE);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    DraggedMusic dm;
    stream >> dm;

    if(dm.action == Qt::CopyAction){
        qDebug() << "Inserting music " << dm.key << " to " << parent.row() << ", " << parent.column();
        qDebug() << "For info, row = " << row << " and column = " << column;
        auto musics = collection.getMusics();
        auto key = dm.key;
        auto mf = musics[key.toStdString()];

        if(parent.row() == -1 && row == -1){
            qDebug() << "Row is invalid, inserting at " << rowCount(parent);
            beginInsertRows(parent, rowCount(parent), rowCount(parent));
            playlist->getMusics().push_back(mf);
        } else {
            qDebug() << "Row is ok, inserting at " << row;
            beginInsertRows(parent, row, row);
            auto it = playlist->getMusics().begin() + row;
            playlist->getMusics().insert(it, {mf});
        }
        endInsertRows();

    } else if(dm.action == Qt::MoveAction){
        qDebug() << "dm.row :" << dm.row << "and dm.column :" << dm.column;
        qDebug() << "row :" << row << " and column :" << column;
        qDebug() << "insert to :" << parent.row() << ", " << parent.column();
        if(parent.row() == -1 && row == -1){
            beginMoveRows(parent, dm.row, dm.row, parent, rowCount(parent));
            auto it = playlist->getMusics().begin() + dm.row;
            std::rotate(it, it + 1, playlist->getMusics().end());

        } else {
            beginMoveRows(QModelIndex(), dm.row, dm.row, QModelIndex(), parent.row());
            //auto mf = playlist->getMusics().at(dm.row);
            //playlist->getMusics().erase(playlist->getMusics().begin()+dm.row);
            //playlist->getMusics().insert(playlist->getMusics().begin()+parent.row(), mf);
            endMoveRows();
        }

    } else {
        qDebug() << "Bad action : " << action;
        return false;
    }
    return true;
}

QMimeData* PlaylistModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    // take only the first index
    for(const auto &index : indexes){
        if (index.isValid()){
            auto item = itemAt(index.row());
            if(item) {
                DraggedMusic dm;
                dm.action = Qt::MoveAction;
                dm.row = index.row();
                dm.column = index.column();
                dm.key = item->getFilepath().c_str();
                stream << dm;
                break;
            }
        }
    }
    mimeData->setData(MIME_TYPE, encodedData);
    return mimeData;
}

Qt::DropActions PlaylistModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions PlaylistModel::supportedDragActions() const {
    return Qt::MoveAction;
}
