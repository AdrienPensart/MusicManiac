#ifndef MUSICFILE_HPP
#define MUSICFILE_HPP

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <string>

class MusicFile
{
    public:

        MusicFile(std::string filepath, TagLib::File *);
        virtual ~MusicFile();
        std::string getDuration();
        std::string getFilepath();
        double getRating();
        std::string getUUID();
        std::string getKeywords();
        bool isModified();
        virtual void setRating(double)=0;
        virtual void setKeywords(std::string)=0;
        virtual void save();

    protected:

        virtual void setKeywords(std::string, bool erase);
        virtual void setRating(double, bool erase);
        virtual void setUUID(std::string, bool erase);

    private:

        std::string filepath;
        TagLib::File * file;
        bool modified;
        double rating;
        std::string keywords;
        std::string uuid;
        std::string duration;
};

#endif // MUSICFILE_HPP
