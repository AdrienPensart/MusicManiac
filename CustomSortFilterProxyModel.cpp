#include "CustomSortFilterProxyModel.hpp"
#include "MusicFolderModel.hpp"
#include <iostream>
using namespace std;

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QStringListModel& _without, QStringListModel& _with, QObject * parent)
    : QSortFilterProxyModel(parent), rating(0), minDuration("00:00"), maxDuration("100:00"), without(_without), with(_with)
{
}

void CustomSortFilterProxyModel::refilter(){
    setFilterFixedString("no-op");
}

void CustomSortFilterProxyModel::ratingChanged(double _rating){
    rating = _rating;
    refilter();
}

void CustomSortFilterProxyModel::minDurationChanged(QString _minDuration){
    minDuration = _minDuration;
    refilter();
}

void CustomSortFilterProxyModel::maxDurationChanged(QString _maxDuration){
    maxDuration = _maxDuration;
    refilter();
}

bool CustomSortFilterProxyModel::filterAcceptsRow (int sourceRow, const QModelIndex& sourceParent) const {
    QAbstractItemModel * model = sourceModel();
    QModelIndex keywords_index = model->index(sourceRow, MusicFolderModel::COLUMN_KEYWORDS, sourceParent);
    QString keywords = model->data(keywords_index).toString();
    foreach(QString keyword, without.stringList()){
        if(keywords.contains(keyword)){
            return false;
        }
    }

    foreach(QString keyword, with.stringList()){
        if(!keywords.contains(keyword)){
            return false;
        }
    }

    QModelIndex rating_index = model->index(sourceRow, MusicFolderModel::COLUMN_RATING, sourceParent);
    double currentRating = model->data(rating_index).toDouble();
    if(currentRating < rating){
        return false;
    }

    QModelIndex duration_index = model->index(sourceRow, MusicFolderModel::COLUMN_DURATION, sourceParent);
    QString currentDuration = model->data(duration_index).toString();
    if(minDuration > currentDuration || maxDuration < currentDuration){
        return false;
    }
    return true;
}
