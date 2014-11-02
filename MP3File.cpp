#include <QDebug>
#include <QUuid>

#include "MP3File.hpp"

#include <taglib/tpropertymap.h>
#include <taglib/id3v2tag.h>
#include <taglib/uniquefileidentifierframe.h>

MP3File::MP3File(QString filepath, TagLib::MPEG::File * _mp3)
    : MusicFile(filepath, _mp3), mp3(_mp3)
{
}

double MP3File::getRating(){
    TagLib::PropertyMap tags = mp3->properties();
    TagLib::StringList list = tags["FMPS_RATING"];
    double rating = 0;
    if(list.size() == 1){
        QString ratingStr = QString::fromStdString(list[0].to8Bit(true));
        rating = ratingStr.toDouble();
        rating *= 5;
    }
    return rating;
}

QString MP3File::getUUID(){
    const TagLib::ID3v2::FrameListMap& frames = mp3->ID3v2Tag()->frameListMap();
    TagLib::ID3v2::FrameListMap::ConstIterator ufidIter = frames.find("UFID");
    QString uuid;
    if(ufidIter == frames.end()){
        qDebug() << "No UFID frame";
        QUuid quuid = QUuid::createUuid();
        uuid = quuid.toString();
        qDebug() << "Generating UUID : " << uuid;
        TagLib::ID3v2::UniqueFileIdentifierFrame * ufid = new TagLib::ID3v2::UniqueFileIdentifierFrame("braincraft", uuid.toStdString().c_str());
        mp3->ID3v2Tag()->addFrame(ufid);
        //mp3.save();
    } else {
        TagLib::ID3v2::UniqueFileIdentifierFrame * ufid = (TagLib::ID3v2::UniqueFileIdentifierFrame *)ufidIter->second.front();
        uuid = ufid->identifier().data();
    }
    return uuid;
}

void MP3File::setKeywords(const QStringList&){
}

QStringList MP3File::getKeywords(){
    return QStringList();
}
