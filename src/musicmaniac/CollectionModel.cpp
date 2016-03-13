#include "CollectionModel.hpp"
#include "DragAndDropMusic.hpp"
#include <QDebug>

CollectionModel::CollectionModel(QObject *parent) :
    QStandardItemModel(parent) {
}

CollectionModel::~CollectionModel() {
}

Qt::ItemFlags CollectionModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled; //QStandardItemModel::flags(index);
    if(index.isValid()){
        auto item = itemFromIndex(index);
        auto type = item->data(CollectionRoles::ItemTypeRole).toString();
        if(type == SONG){
            return Qt::ItemIsDragEnabled | defaultFlags;
        }
    }
    return defaultFlags;
}

QStringList CollectionModel::mimeTypes() const {
    QStringList types;
    types << MIME_TYPE;
    return types;
}

QMimeData* CollectionModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    // take only the first index available
    for(const auto &index : indexes){
        if (index.isValid()){
            auto item = itemFromIndex(index);
            DraggedMusic dm;
            dm.action = Qt::CopyAction;
            dm.row = index.row();
            dm.column = index.column();
            dm.key = item->data(CollectionRoles::ItemKey).toString();
            stream << dm;
        }
    }
    mimeData->setData(MIME_TYPE, encodedData);
    return mimeData;
}
