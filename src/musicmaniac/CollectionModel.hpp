#pragma once

#include <QStandardItemModel>

const QString MIME_TYPE = "application/string.path";

class CollectionModel : public QStandardItemModel {
    Q_OBJECT
    public:
        explicit CollectionModel(QObject *parent = 0);
        virtual ~CollectionModel();
        QMimeData* mimeData(const QModelIndexList &indexes) const;
        QStringList mimeTypes() const;
};

