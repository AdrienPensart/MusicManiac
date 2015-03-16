#include <taglib/id3v2tag.h>

#include "MusicFileFactory.hpp"
#include "MP3File.hpp"
#include "FLACFile.hpp"
#include "MusicDebugger.hpp"

#include <boost/filesystem.hpp>
#include <boost/lambda/bind.hpp>

MusicFileFactory::MusicFileFactory(QString folder, bool _regen) :
    iterator(folder, QDirIterator::Subdirectories),
    regen(_regen)
{
    readCount = 0;
    totalCount = dir.count();
    using namespace boost::filesystem;
    using namespace boost::lambda;

    LOG << "Counting files in " + folder.toStdString();
    path the_path(folder.toStdString());
    for(recursive_directory_iterator it(folder.toStdString()); it != recursive_directory_iterator(); ++it)
    {
       totalCount++;
    } 
    LOG << "Total count of files : " + Common::toString(totalCount);
}
        
int MusicFileFactory::getTotalCount(){
    return totalCount;
}

int MusicFileFactory::getReadCount(){
    return readCount;
}

double MusicFileFactory::progression(){
    return (double)readCount / (double)totalCount;
}

bool MusicFileFactory::valid(){
    return iterator.hasNext();
}

const std::vector<std::string>& MusicFileFactory::getPlaylists(){
    return playlists;
}

MusicFile * MusicFileFactory::factory(){
    while(iterator.hasNext()){
        iterator.next();
        readCount++;
        if(iterator.fileInfo().isDir()){
            continue;
        }
        MusicDebugger::instance().setCurrentMusic(iterator.filePath().toStdString());

        if(iterator.filePath().endsWith(".mp3")){
            TagLib::MPEG::File * mp3 = new TagLib::MPEG::File(iterator.filePath().toStdString().c_str());
            if(!mp3->audioProperties()){
                LOG << "No audio property for " + iterator.filePath().toStdString();
                continue;
            }

            if(!mp3->hasID3v2Tag()){
                LOG << "No ID3v2 Tag present for " + iterator.filePath().toStdString();
                continue;
            }

            TagLib::ID3v2::Tag * tag = mp3->ID3v2Tag();
            if(!tag){
                LOG << "Tag invalid for " + iterator.filePath().toStdString();
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
                LOG << "No audio property for " + iterator.filePath().toStdString();
                continue;
            }

            if(!flac->hasXiphComment()){
                LOG << "No XiphComment present for " + iterator.filePath().toStdString();
                continue;
            }

            TagLib::Ogg::XiphComment * tag = flac->xiphComment();
            if(!tag){
                LOG << "Tag invalid for " + iterator.filePath().toStdString();
                continue;
            }
            return new FLACFile(iterator.filePath().toStdString(), flac, regen);
        } else if(iterator.filePath().endsWith(".m3u")){
            playlists.push_back(iterator.filePath().toStdString());
        } else {
            //LOG << "Music file not supported " + iterator.filePath().toStdString();
        }
    }
    return 0;
}
