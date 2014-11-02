#ifndef FLACFILE_H
#define FLACFILE_H

#include "MusicFile.hpp"
#include <taglib/flacfile.h>

class FLACFile : public MusicFile
{
    public:

        FLACFile(QString filepath, TagLib::FLAC::File * flac);
        virtual double getRating();
        virtual QString getUUID();
        virtual void setKeywords(const QStringList&);
        virtual QStringList getKeywords();

     private:

        TagLib::FLAC::File * flac;
};

#endif // FLACFILE_H
