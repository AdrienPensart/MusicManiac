#include "ArtistModel.hpp"
#include "common/Utility.hpp"
#include <set>
#include <QDebug>

ArtistModel::ArtistModel(QObject *parent) :
    QAbstractTableModel(parent) {
    musicsCount = 0;
    albumsCount = 0;
    playlistsCount = 0;
    averageRating = 0;
}

ArtistModel::~ArtistModel() {
}

void ArtistModel::set(QString _artist, const Musics& musics, const MusicsByAlbums& albums, const PlaylistVector& playlists) {
    beginResetModel();
    artist = _artist;
    musicsCount = musics.size();
    albumsCount = albums.size();
    playlistsCount = playlists.size();

    std::set<std::string> genresSet;
    std::set<std::string> keywordsSet;
    int totalSeconds = 0;
    averageRating = 0;
    for(const auto& song : musics){
        genresSet.insert(song.second->getGenre());
        auto songKeywords = song.second->getSplittedKeywords();
        keywordsSet.insert(songKeywords.begin(), songKeywords.end());
        totalSeconds += song.second->getDurationInSeconds();
        averageRating += song.second->getRating();
    }

    genres = QString::fromStdString(Common::implode(genresSet));
    keywords = QString::fromStdString(Common::implode(keywordsSet));
    duration = QString::fromStdString(secondsToString(totalSeconds));
    averageRating /= (double)musicsCount;
    endResetModel();
}

int ArtistModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : ROW_COUNT;
}

int ArtistModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : COLUMN_COUNT;
}

Qt::ItemFlags ArtistModel::flags(const QModelIndex &index) const {
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

QVariant ArtistModel::data(const QModelIndex& index, int role) const {
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
            case ROW_NAME:
                return index.column() == 0 ? tr("Name") : artist;
                break;
            case ROW_MUSICS_COUNT:
                return index.column() == 0 ? tr("Musics count") : QString::number(musicsCount);
                break;
            case ROW_ALBUMS_COUNT:
                return index.column() == 0 ? tr("Albums count") : QString::number(albumsCount);
                break;
            case ROW_PLAYLISTS_COUNT:
                return index.column() == 0 ? tr("Playlists count") : QString::number(playlistsCount);
                break;
            case ROW_AVR_RATING:
                return index.column() == 0 ? tr("Average rating") : QString::number(averageRating);
                break;
            case ROW_KEYWORDS:
                return index.column() == 0 ? tr("Keywords") : keywords;
                break;
            case ROW_GENRES:
                return index.column() == 0 ? tr("Genres") : genres;
                break;
            case ROW_DURATION:
                return index.column() == 0 ? tr("Duration") : duration;
                break;
            default:
                return tr("Undefined");
        }
    }
    return tr("Undefined");
}

bool ArtistModel::setData (const QModelIndex & index, const QVariant & value, int role) {
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

QVariant ArtistModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
