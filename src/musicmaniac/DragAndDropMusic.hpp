#pragma once

#include <QMimeData>
#include <QDataStream>
const QString MIME_TYPE = "application/string.path";

struct DraggedMusic
{
    DraggedMusic();
    Qt::DropAction action;
    int row;
    int column;
    QString key;
};

QDataStream &operator<<(QDataStream & stream, const DraggedMusic& dm);
QDataStream &operator>>(QDataStream & stream, DraggedMusic& dm);
