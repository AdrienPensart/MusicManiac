#include "MusicFolderModel.hpp"
#include "Utility.hpp"
#include <QSet>
#include <iostream>
using namespace std;

MusicFolderModel::MusicFolderModel(QObject *parent) :
    QAbstractTableModel(parent) {
}

MusicFolderModel::~MusicFolderModel(){
    clear();
}

QStringList MusicFolderModel::getKeywords(){
    QStringList keywords;
    for(QVector<MusicFile*>::ConstIterator ci = musics.begin(); ci != musics.end() ; ci++){
        vector<string> currentKeywords;
        Common::split((*ci)->getKeywords(), " ", currentKeywords);
        for(vector<string>::const_iterator si = currentKeywords.begin(); si != currentKeywords.end(); si++){
            keywords.append(QString::fromStdString(*si));
        }
    }    
    QStringList sorted = keywords.toSet().toList();
    sorted.sort();
    return sorted;
}

void MusicFolderModel::clear(){
    beginResetModel();
    cout << "Clearing music folder" << endl;
    for(QVector<MusicFile*>::ConstIterator ci = musics.begin(); ci != musics.end() ; ci++){
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
    musics.append(mf);
    endInsertRows();
}

MusicFile * MusicFolderModel::musicAt(int row) const {
    if(row >= 0 && row < (int)musics.size())
    {
        return musics[row];
    }
    return 0;
}

Qt::ItemFlags MusicFolderModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    if(index.column() == COLUMN_KEYWORDS || index.column() == COLUMN_RATING){
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    return QAbstractItemModel::flags(index);// | Qt::ItemIsEditable;
}

QVariant MusicFolderModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()){
        return QVariant();
    }

    if (index.row() >= musics.size()){
        return QVariant();
    }
    /*
    if(role == Qt::TextAlignmentRole) {
        return int(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    */

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        MusicFile * rowMusic = musicAt(index.row());
        if(rowMusic != 0){
            return infoAtColumn(rowMusic, index.column());
        }
    }
    return QVariant();
}

bool MusicFolderModel::setData (const QModelIndex & index, const QVariant & value, int role){
    if (index.isValid() && role == Qt::EditRole){
        cout << "Setting data : " << value.toString().toStdString() << endl;
        MusicFile * rowMusic = musicAt(index.row());
        if(rowMusic){
            switch(index.column()){
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
            case COLUMN_PATH:
                return tr("Path");
                break;
            case COLUMN_UUID:
                return tr("UUID");
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
        }
    }
    return QVariant();
}

QVariant MusicFolderModel::infoAtColumn(MusicFile * mf, int row) const
{
    if(!mf){
        return "Undefined";
    }

    switch(row){
        case COLUMN_PATH:
            return mf->getFilepath().c_str();
            break;
        case COLUMN_UUID:
            return mf->getUUID().c_str();
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
