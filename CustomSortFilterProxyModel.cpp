#include "CustomSortFilterProxyModel.hpp"
#include "MusicFolderModel.hpp"
#include <iostream>
using namespace std;

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QStringListModel& _without, QStringListModel& _with, QObject * parent)
    : QSortFilterProxyModel(parent), rating(0), without(_without), with(_with)
{
}

void CustomSortFilterProxyModel::refilter(){
    setFilterFixedString("no-op");
}

void CustomSortFilterProxyModel::ratingChanged(double _rating){
    rating = _rating;
    refilter();
}

bool CustomSortFilterProxyModel::filterAcceptsRow (int sourceRow, const QModelIndex& sourceParent) const {
    QAbstractItemModel * model = sourceModel();

    QModelIndex path_index = model->index(sourceRow, MusicFolderModel::COLUMN_PATH, sourceParent);
    QModelIndex keywords_index = model->index(sourceRow, MusicFolderModel::COLUMN_KEYWORDS, sourceParent);

    QString path = model->data(path_index).toString();
    QString keywords = model->data(keywords_index).toString();
    cout << "For " << path.toStdString() << endl;

    foreach(QString keyword, without.stringList()){
        if(keywords.contains(keyword)){
            cout << "INVALID : " << keywords.toStdString() << " contains a without keyword : " << keyword.toStdString() << endl << endl;
            return false;
        } else {
            cout << keywords.toStdString() << " does not contains a without keyword : " << keyword.toStdString() << endl;
        }
    }

    foreach(QString keyword, with.stringList()){
        if(!keywords.contains(keyword)){
            cout << "INVALID : " << keywords.toStdString() << " does not contains a with keyword : " << keyword.toStdString() << endl << endl;
            return false;
        } else {
            cout << keywords.toStdString() << " contains a with keyword : " << keyword.toStdString() << endl;
        }
    }

    QModelIndex rating_index = model->index(sourceRow, MusicFolderModel::COLUMN_RATING, sourceParent);
    double current = model->data(rating_index).toDouble();
    if(current < rating){
        cout << "INVALID : rating is below" << endl;
        return false;
    }

    cout << keywords.toStdString() << " is a valid track" << endl << endl;
    return true;
}
