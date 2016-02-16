#include "CollectionModel.hpp"

#include <QMimeData>
#include <QDebug>

const QString MIME_TYPE = "application/string.path";

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
            qDebug() << "it is a song, enabling drag";
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

    for(const auto &index : indexes){
        if (index.isValid()){
            auto item = itemFromIndex(index);
            auto musicKey = item->data(CollectionRoles::ItemKey).toString();
            stream << musicKey;
            mimeData->setText(musicKey);
        }
    }
    mimeData->setData(MIME_TYPE, encodedData);
    return mimeData;
}
