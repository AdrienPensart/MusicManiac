#include <QDebug>

#include "MusicFileFactory.hpp"
#include "MP3File.hpp"
#include "FLACFile.hpp"

MusicFileFactory::MusicFileFactory(QString folder)
    : music(folder), iterator(music.absolutePath(), QDirIterator::Subdirectories) {
}

bool MusicFileFactory::valid(){
    return iterator.hasNext();
}

MusicFile * MusicFileFactory::factory(){
    while(iterator.hasNext()){
        iterator.next();
        qDebug() << "Analyzing : " << iterator.filePath();
        if(iterator.fileInfo().isDir()){
            continue;
        }

        if(iterator.filePath().endsWith(".mp3")){
            TagLib::MPEG::File * mp3 = new TagLib::MPEG::File(iterator.filePath().toStdString().c_str());
            if(!mp3->audioProperties()){
                qDebug() << "No audio property";
                continue;
            }

            if(!mp3->hasID3v2Tag()){
                qDebug() << "No ID3v2 Tag present";
                continue;
            }

            TagLib::ID3v2::Tag * tag = mp3->ID3v2Tag();
            if(!tag){
                qDebug() << "Tag invalid";
                continue;
            }
            qDebug() << "Building mp3 " << iterator.filePath();
            return new MP3File(iterator.filePath(), mp3);
        }
        else if(iterator.filePath().endsWith(".flac")){
            TagLib::FLAC::File * flac = new TagLib::FLAC::File(iterator.filePath().toStdString().c_str());
            if(!flac->audioProperties()){
                qDebug() << "No audio property";
                continue;
            }

            if(!flac->hasXiphComment()){
                qDebug() << "No XiphComment present";
                continue;
            }

            TagLib::Ogg::XiphComment * tag = flac->xiphComment();
            if(!tag){
                qDebug() << "Tag invalid";
                continue;
            }
            qDebug() << "Building flac " << iterator.filePath();
            return new FLACFile(iterator.filePath(), flac);
        } else {
            qDebug() << "Music file not supported " << iterator.filePath();
        }
    }
    return 0;
}
