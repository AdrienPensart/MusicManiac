#pragma once

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <vector>
#include <string>

#include <log/Log.hpp>

class MusicFile
{
    public:

        MusicFile(std::string filepath, TagLib::File *);
        virtual ~MusicFile();
        std::string getDuration() const;
        unsigned int getDurationInSeconds() const;
        std::string getFilepath() const;
        double getRating() const;
        std::string getUUID() const;
        std::string getKeywords() const;
        std::string getArtist() const;
        std::vector<std::string> getSplittedKeywords() const;
        bool isModified();
        virtual void setRating(double)=0;
        virtual void setKeywords(std::string)=0;
        virtual void save();

    protected:

        void touch();
        virtual void setKeywords(std::string, bool erase);
        virtual void setRating(double, bool erase);
        virtual void setUUID(std::string, bool erase);
        virtual void setArtist(std::string, bool erase);

    private:

        int totalSeconds;
        std::string filepath;
        TagLib::File * file;
        bool modified;
        double rating;
        std::string artist;
        std::string keywords;
        std::string uuid;
        std::string duration;
};
