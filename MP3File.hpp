#ifndef MP3FILE_HPP
#define MP3FILE_HPP

#include "MusicFile.hpp"
#include <taglib/mpegfile.h>

class MP3File : public MusicFile
{
    public:

        MP3File(std::string filepath, TagLib::MPEG::File * mp3);
        virtual void setKeywords(std::string);
        virtual void setRating(double);

    private:

        TagLib::MPEG::File * mp3;
};

#endif // MP3FILE_HPP
