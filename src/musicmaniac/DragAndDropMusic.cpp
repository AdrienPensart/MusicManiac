#include "DragAndDropMusic.hpp"

DraggedMusic::DraggedMusic() : row(-1), column(-1){

}

QDataStream &operator<<(QDataStream & stream, const DraggedMusic& dm){
    stream << (qint32)dm.action;
    stream << dm.row;
    stream << dm.column;
    stream << dm.key;
}

QDataStream &operator>>(QDataStream & stream, DraggedMusic& dm){
    qint32 action, row, column;
    stream >> action;
    dm.action = (Qt::DropAction)action;
    stream >> dm.row;
    stream >> dm.column;
    stream >> dm.key;
}
