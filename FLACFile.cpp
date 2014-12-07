#include "FLACFile.hpp"

#include <common/Utility.hpp>
#include "Uuid.hpp"

#include <taglib/xiphcomment.h>
#include <taglib/tstringlist.h>
#include <taglib/tpropertymap.h>

FLACFile::FLACFile(std::string filepath, TagLib::FLAC::File * _flac, bool regen)
    : MusicFile(filepath, _flac), flac(_flac) {
    double rating = 0;
    if(!flac->xiphComment()->contains("FMPS_RATING")){
        LOG << "No rating in file " + filepath;
    } else {
        TagLib::StringList list = flac->xiphComment()->properties()["FMPS_RATING"];
        if(list.size()){
            std::string ratingStr = list[0].to8Bit(true);
            Common::fromString(ratingStr, rating);
            rating *= 5;

        }
    }
    MusicFile::setRating(rating, false);

    if(regen || !flac->xiphComment()->contains("UFID")){
        std::string uuid = newUUID();
        flac->xiphComment()->addField("UFID", uuid.c_str());
        LOG << "No UFID frame for " + getFilepath() + " generating one : " + uuid;
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
        flac->xiphComment()->addField("FMPS_RATING", Common::toString(_rating / 5.0).c_str());
    } else {
        flac->xiphComment()->addField("FMPS_RATING", Common::toString(_rating / 5.0).c_str(), true);
    }
    MusicFile::setRating(_rating, true);
}

void FLACFile::setKeywords(std::string _keywords){
    flac->xiphComment()->setComment(_keywords.c_str());
    MusicFile::setKeywords(_keywords, true);
}
