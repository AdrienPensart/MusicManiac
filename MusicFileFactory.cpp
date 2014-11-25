#include <iostream>
using namespace std;

#include <taglib/id3v2tag.h>

#include "MusicFileFactory.hpp"
#include "MP3File.hpp"
#include "FLACFile.hpp"
#include "MusicDebugger.hpp"

MusicFileFactory::MusicFileFactory(QString folder, bool _regen) :
    music(folder), iterator(music.absolutePath(), QDirIterator::Subdirectories),
    regen(_regen)
{
}

bool MusicFileFactory::valid(){
    return iterator.hasNext();
}

MusicFile * MusicFileFactory::factory(){
    while(iterator.hasNext()){
        iterator.next();
        if(iterator.fileInfo().isDir()){
            continue;
        }
        MusicDebugger::instance().setCurrentMusic(iterator.filePath().toStdString());

        if(iterator.filePath().endsWith(".mp3")){
            TagLib::MPEG::File * mp3 = new TagLib::MPEG::File(iterator.filePath().toStdString().c_str());
            if(!mp3->audioProperties()){
                cout << "No audio property for " << iterator.filePath().toStdString() << endl;
                continue;
            }

            if(!mp3->hasID3v2Tag()){
                cout << "No ID3v2 Tag present for " << iterator.filePath().toStdString() << endl;
                continue;
            }

            TagLib::ID3v2::Tag * tag = mp3->ID3v2Tag();
            if(!tag){
                cout << "Tag invalid for " << iterator.filePath().toStdString() << endl;
                continue;
            }

            if(regen){
                tag->removeFrames("UFID");
                mp3->save();

                // reopen file
                delete mp3;
                mp3 = new TagLib::MPEG::File(iterator.filePath().toStdString().c_str());
            }
            return new MP3File(iterator.filePath().toStdString(), mp3);
        }
        else if(iterator.filePath().endsWith(".flac")){
            TagLib::FLAC::File * flac = new TagLib::FLAC::File(iterator.filePath().toStdString().c_str());
            if(!flac->audioProperties()){
                cout << "No audio property for " << iterator.filePath().toStdString() << endl;
                continue;
            }

            if(!flac->hasXiphComment()){
                cout << "No XiphComment present for " << iterator.filePath().toStdString() << endl;
                continue;
            }

            TagLib::Ogg::XiphComment * tag = flac->xiphComment();
            if(!tag){
                cout << "Tag invalid for " << iterator.filePath().toStdString() << endl;
                continue;
            }
            return new FLACFile(iterator.filePath().toStdString(), flac, regen);
        } else {
            cout << "Music file not supported " << iterator.filePath().toStdString() << endl;
        }
    }
    return 0;
}
