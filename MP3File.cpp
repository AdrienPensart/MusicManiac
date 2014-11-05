#include <QDebug>
#include <QUuid>
#include <iostream>

#include "MP3File.hpp"

#include <taglib/tpropertymap.h>
#include <taglib/id3v2tag.h>
#include <taglib/uniquefileidentifierframe.h>
#include <taglib/textidentificationframe.h>

MP3File::MP3File(QString filepath, TagLib::MPEG::File * _mp3)
    : MusicFile(filepath, _mp3), mp3(_mp3)
{
    double rating = 0;
    TagLib::StringList list = mp3->properties()["FMPS_RATING"];
    if(list.size() == 1){
        QString ratingStr = QString::fromStdString(list[0].to8Bit(true));
        rating = ratingStr.toDouble();
        rating *= 5;
    }
    setRating(rating);

    const TagLib::ID3v2::FrameListMap& frames = mp3->ID3v2Tag()->frameListMap();
    TagLib::ID3v2::FrameListMap::ConstIterator ufidIter = frames.find("UFID");
    QString uuid;
    if(ufidIter == frames.end()){
        QUuid quuid = QUuid::createUuid();
        uuid = quuid.toString();
        qDebug() << "No UFID frame for " << getFilepath() << " generating one " << uuid;
        TagLib::ID3v2::UniqueFileIdentifierFrame * ufid = new TagLib::ID3v2::UniqueFileIdentifierFrame("braincraft", uuid.toStdString().c_str());
        mp3->ID3v2Tag()->addFrame(ufid);
    } else {
        TagLib::ID3v2::UniqueFileIdentifierFrame * ufid = (TagLib::ID3v2::UniqueFileIdentifierFrame *)ufidIter->second.front();
        uuid = ufid->identifier().data();
    }
    setUUID(uuid);
    setKeywords(mp3->ID3v2Tag()->comment().to8Bit(true).c_str());
}

void MP3File::setRating(double _rating){
    MusicFile::setRating(_rating);
    /*
    TagLib::StringList& list = mp3->properties()["FMPS_RATING"];
    if(list.size() == 1){
        list[0] = QString::number(_rating).toStdString().c_str();
    }
    */
}

void MP3File::setKeywords(QString _keywords){
    MusicFile::setKeywords(_keywords);
    mp3->ID3v2Tag()->setComment(_keywords.toStdString().c_str());
}
