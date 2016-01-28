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
    return parent.isValid() ? 0 : 1;
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
        switch(index.column()){
            case COLUMN_NAME:
                return artist;
                break;
            case COLUMN_MUSICS_COUNT:
                return QString::number(musicsCount);
                break;
            case COLUMN_ALBUMS_COUNT:
                return QString::number(albumsCount);
                break;
            case COLUMN_PLAYLISTS_COUNT:
                return QString::number(playlistsCount);
                break;
            case COLUMN_AVR_RATING:
                return QString::number(averageRating);
                break;
            case COLUMN_KEYWORDS:
                return keywords;
                break;
            case COLUMN_GENRES:
                return genres;
                break;
            case COLUMN_DURATION:
                return duration;
                break;
            default:
                return tr("Undefined");
        }
    }
    return QVariant();
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
            case COLUMN_NAME:
                return tr("Name");
                break;
            case COLUMN_MUSICS_COUNT:
                return tr("Musics count");
                break;
            case COLUMN_ALBUMS_COUNT:
                return tr("Albums count");
                break;
            case COLUMN_PLAYLISTS_COUNT:
                return tr("Playlists count");
                break;
            case COLUMN_AVR_RATING:
                return tr("Average rating");
                break;
            case COLUMN_KEYWORDS:
                return tr("Keywords");
                break;
            case COLUMN_GENRES:
                return tr("Genres");
                break;
            case COLUMN_DURATION:
                return tr("Duration");
                break;
            default:
                return tr("Undefined");
        }
    }
    return QVariant();
}
