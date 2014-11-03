#ifndef MP3FILE_HPP
#define MP3FILE_HPP

#include "MusicFile.hpp"
#include <taglib/mpegfile.h>

class MP3File : public MusicFile
{
    public:

        MP3File(QString filepath, TagLib::MPEG::File * mp3);
        virtual void setKeywords(const QStringList&);

    private:

        TagLib::MPEG::File * mp3;
};

#endif // MP3FILE_HPP
