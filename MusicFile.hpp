#pragma once

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <string>

#include <log/Log.hpp>

class MusicFile
{
    public:

        MusicFile(std::string filepath, TagLib::File *);
        virtual ~MusicFile();
        std::string getDuration() const;
        std::string getFilepath() const;
        double getRating() const;
        std::string getUUID() const;
        std::string getKeywords() const;
        bool isModified();
        virtual void setRating(double)=0;
        virtual void setKeywords(std::string)=0;
        virtual void save();

    protected:

        void touch();
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
