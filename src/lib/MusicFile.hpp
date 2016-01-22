#pragma once

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <vector>
#include <string>
#include <map>

class MusicFile;

typedef std::map<std::string, MusicFile*> Musics;
typedef std::map<std::string, Musics > Artists;
typedef std::map<std::string, Musics > Albums;
typedef std::map<std::string, Musics > Keywords;
typedef std::map<std::string, Musics > Genres;
typedef std::map<std::string, Keywords > KeywordsByArtist;
typedef std::vector<MusicFile*> MusicVector;
typedef std::vector<std::pair<std::string, MusicVector>> MusicVectorByAlbum;
typedef std::vector<std::pair<std::string, MusicVectorByAlbum>> Tree;

class MusicFile {
    public:
        MusicFile(const std::string& filepath, TagLib::File *);
        virtual ~MusicFile();
        std::string getDuration() const;
        unsigned int getDurationInSeconds() const;
        std::string getPath() const;
        const std::string& getFilepath() const;
        std::string getFilename() const;
        double getRating() const;
        std::string getUUID() const;
        std::string getKeywords() const;
        std::string getArtist() const;
        std::string getGenre() const;
        std::string getYoutube() const;
        std::string getTitle() const;
        std::string getAlbum() const;
        void setYoutube(std::string);
        std::vector<std::string> getSplittedKeywords() const;
        bool isModified();
        virtual void setRating(double)=0;
        virtual void setKeywords(std::string)=0;
        virtual void setGenre(std::string)=0;
        virtual void setTitle(std::string)=0;
        virtual void setAlbum(std::string)=0;
        virtual void setArtist(std::string)=0;
        virtual void save();

    protected:

        void touch();
        virtual void setKeywords(std::string, bool erase);
        virtual void setRating(double, bool erase);
        virtual void setUUID(std::string, bool erase);
        virtual void setArtist(std::string, bool erase);
        virtual void setGenre(std::string, bool erase);
        virtual void setTitle(std::string, bool erase);
        virtual void setAlbum(std::string, bool erase);

    private:

        int totalSeconds;
        std::string filepath;
        TagLib::File * file;
        bool modified;
        double rating;
        std::string title;
        std::string genre;
        std::string artist;
        std::string keywords;
        std::string uuid;
        std::string duration;
        std::string youtube;
        std::string album;
};
