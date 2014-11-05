#include <QDebug>
#include <QUuid>

#include "FLACFile.hpp"

#include <taglib/xiphcomment.h>
#include <taglib/tstringlist.h>
#include <taglib/tpropertymap.h>

FLACFile::FLACFile(QString filepath, TagLib::FLAC::File * _flac)
    : MusicFile(filepath, _flac), flac(_flac)
{
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
    setRating(rating);

    QString uuid;
    if(!flac->xiphComment()->contains("UFID")){
        QUuid quuid = QUuid::createUuid();
        uuid = quuid.toString();
        flac->xiphComment()->addField("UFID", uuid.toStdString().c_str());
        qDebug() << "No UFID frame for " << getFilepath() << " generating one " << uuid;
    } else {
        TagLib::StringList list = flac->xiphComment()->properties()["UFID"];
        if(list.size()){
            uuid = list.front().toCString();
        }
    }
    setUUID(uuid);
}

void FLACFile::setKeywords(QString _keywords){
    MusicFile::setKeywords(_keywords);
}
