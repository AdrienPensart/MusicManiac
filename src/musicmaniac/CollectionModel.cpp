#include "CollectionModel.hpp"

#include <QMimeData>

CollectionModel::CollectionModel(QObject *parent) :
    QStandardItemModel(parent) {
}

CollectionModel::~CollectionModel() {
}

QStringList CollectionModel::mimeTypes() const
{
    QStringList types;
    types << MIME_TYPE;
    return types;
}

QMimeData* CollectionModel::mimeData(const QModelIndexList &indexes) const{
    qDebug() << "Encoding data";

    QMimeData* mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for(const auto &index : indexes){
        if (index.isValid()){
            QString text = data(index, Qt::DisplayRole).toString();
            stream << text;
        }
    }
    mimeData->setData(MIME_TYPE, encodedData);
    return mimeData;
}
