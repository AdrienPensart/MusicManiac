#ifndef FLACFILE_H
#define FLACFILE_H

#include "MusicFile.hpp"
#include <taglib/flacfile.h>

class FLACFile : public MusicFile
{
    public:

        FLACFile(QString filepath, TagLib::FLAC::File * flac);
        virtual void setKeywords(QString);

     private:

        TagLib::FLAC::File * flac;
};

#endif // FLACFILE_H
