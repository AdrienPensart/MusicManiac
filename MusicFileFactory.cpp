#include <taglib/id3v2tag.h>

#include "MusicFileFactory.hpp"
#include "MP3File.hpp"
#include "FLACFile.hpp"
#include "MusicDebugger.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lambda/bind.hpp>
using namespace boost::filesystem;
using namespace boost::lambda;

MusicFileFactory::MusicFileFactory(const std::string& _folder, bool _regen) :
    folder(_folder),
    iterator(_folder),
    regen(_regen),
    totalCount(0),
    readCount(0)
{
    LOG << "Constructing directory " + folder;
    try{
        for(recursive_directory_iterator it(folder); it != recursive_directory_iterator(); ++it)
        {
           totalCount++;
        }
        LOG << "Total count of files in " + folder + " : " + Common::toString(totalCount);
    } catch (boost::filesystem::filesystem_error& fex) {
        LOG << "Exception " + std::string(fex.what());
    }
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
    return iterator != recursive_directory_iterator();
}

const std::vector<std::string>& MusicFileFactory::getPlaylists(){
    return playlists;
}

MusicFile * MusicFileFactory::factory(){
    if(!valid()){
        return 0;
    }

    readCount++;
    MusicDebugger::instance().setCurrentMusic(iterator->path().native());
    MusicFile * mf = 0;
    if(boost::algorithm::ends_with(iterator->path().native(), ".mp3")){
        TagLib::MPEG::File * mp3 = new TagLib::MPEG::File(iterator->path().c_str());
        if(!mp3->audioProperties()){
            LOG << "No audio property";
        } else if(!mp3->hasID3v2Tag()){
            LOG << "No ID3v2 Tag present";
        } else {
            TagLib::ID3v2::Tag * tag = mp3->ID3v2Tag();
            if(!tag){
                LOG << "Tag invalid";
            } else if(regen){
                tag->removeFrames("UFID");
                mp3->save();
                // reopen file
                delete mp3;
                mp3 = new TagLib::MPEG::File(iterator->path().c_str());
            }
            mf = new MP3File(iterator->path().native(), mp3);
        }
    }
    else if(boost::algorithm::ends_with(iterator->path().native(), ".flac")){
        TagLib::FLAC::File * flac = new TagLib::FLAC::File(iterator->path().c_str());
        if(!flac->audioProperties()){
            LOG << "No audio property";
        } else if(!flac->hasXiphComment()){
            LOG << "No XiphComment present";
        } else {
            TagLib::Ogg::XiphComment * tag = flac->xiphComment();
            if(!tag){
                LOG << "Tag invalid";
            } else {
                mf = new FLACFile(iterator->path().native(), flac, regen);
            }
        }
    } else if(boost::algorithm::ends_with(iterator->path().native(), ".m3u")){
        playlists.push_back(iterator->path().native());
    } else {
        //LOG << "Music file not supported " + iterator.filePath().toStdString();
    }
    ++iterator;
    return mf;
}
