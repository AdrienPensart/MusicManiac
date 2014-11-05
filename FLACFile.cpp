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
        qDebug() << "No rating in file " << filepath;
    } else {
        TagLib::StringList list = flac->xiphComment()->properties()["FMPS_RATING"];
        if(list.size()){
            QString ratingStr = QString::fromStdString(list[0].to8Bit(true));
            rating = ratingStr.toDouble();
            rating *= 5;
            MusicFile::setRating(rating, false);
        }
    }

    if(!flac->xiphComment()->contains("UFID")){
        QUuid quuid = QUuid::createUuid();
        QString uuid = quuid.toString();
        flac->xiphComment()->addField("UFID", uuid.toStdString().c_str());
        qDebug() << "No UFID frame for " << getFilepath() << " generating one " << uuid;
        MusicFile::setUUID(uuid, true);
    } else {
        TagLib::StringList list = flac->xiphComment()->properties()["UFID"];
        if(list.size()){
            MusicFile::setUUID(list.front().toCString(), false);
        }
    }

    MusicFile::setKeywords(flac->xiphComment()->comment().to8Bit(true).c_str(), false);
}

void FLACFile::setRating(double _rating){
    if(!flac->xiphComment()->contains("FMPS_RATING")){
        flac->xiphComment()->addField("FMPS_RATING", QString::number(_rating / 5.0).toStdString().c_str());
    } else {
        flac->xiphComment()->addField("FMPS_RATING", QString::number(_rating / 5.0).toStdString().c_str(), true);
    }
    MusicFile::setRating(_rating, true);
}

void FLACFile::setKeywords(QString _keywords){
    flac->xiphComment()->setComment(_keywords.toStdString().c_str());
    MusicFile::setKeywords(_keywords, true);
}
