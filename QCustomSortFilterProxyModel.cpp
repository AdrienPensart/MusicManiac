#include "QCustomSortFilterProxyModel.hpp"
#include "MusicFolderModel.hpp"
#include <iostream>
using namespace std;

QCustomSortFilterProxyModel::QCustomSortFilterProxyModel(QStringListModel& _without, QStringListModel& _with, QObject * parent)
    : QSortFilterProxyModel(parent), without(_without), with(_with)
{
}

void QCustomSortFilterProxyModel::refilter(){
    setFilterFixedString("no-op");
}

bool QCustomSortFilterProxyModel::filterAcceptsRow (int sourceRow, const QModelIndex& sourceParent) const {
    QAbstractItemModel * model = sourceModel();

    QModelIndex path_index = model->index(sourceRow, MusicFolderModel::COLUMN_PATH, sourceParent);
    QModelIndex keywords_index = model->index(sourceRow, MusicFolderModel::COLUMN_KEYWORDS, sourceParent);

    QString path = model->data(path_index, Qt::DisplayRole).toString();
    QString keywords = model->data(keywords_index, Qt::DisplayRole).toString();
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

    cout << keywords.toStdString() << " is a valid track" << endl << endl;
    return true;
}
