#include <QDebug>
#include <QUuid>

#include "FLACFile.hpp"

#include <taglib/xiphcomment.h>
#include <taglib/tstringlist.h>
#include <taglib/tpropertymap.h>

FLACFile::FLACFile(QString filepath, TagLib::FLAC::File * _flac)
    : MusicFile(filepath, _flac), flac(_flac)
{
}

double FLACFile::getRating(){
    double rating = 0;
    if(!flac->xiphComment()->contains("FMPS_RATING")){
        qDebug() << "No rating";
    } else {
        TagLib::StringList list = flac->xiphComment()->properties()["FMPS_RATING"];
        if(list.size()){
            QString ratingStr = QString::fromStdString(list[0].to8Bit(true));
            rating = ratingStr.toDouble();
            rating *= 5;
        }
    }
    return rating;
}

QString FLACFile::getUUID(){
    QString uuid;
    if(!flac->xiphComment()->contains("UFID")){
        qDebug() << "No UFID frame";
        QUuid quuid = QUuid::createUuid();
        uuid = quuid.toString();
        flac->xiphComment()->addField("UFID", uuid.toStdString().c_str());
        //flac.save();
    } else {
        TagLib::StringList list = flac->xiphComment()->properties()["UFID"];
        if(list.size()){
            uuid = list.front().toCString();
        }
    }
    return uuid;
}

void FLACFile::setKeywords(const QStringList&){
}

QStringList FLACFile::getKeywords(){
    return QStringList();
}
