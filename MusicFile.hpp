#ifndef MUSICFILE_HPP
#define MUSICFILE_HPP

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <QStringList>

class MusicFile
{
    public:

        MusicFile(QString filepath, TagLib::File *);
        virtual ~MusicFile();
        QString getDuration();
        QString getFilepath();
        double getRating();
        QString getUUID();
        QString getKeywords();
        bool isModified();
        virtual void setRating(double)=0;
        virtual void setKeywords(QString)=0;
        virtual void save();

    protected:

        virtual void setKeywords(QString, bool erase);
        virtual void setRating(double, bool erase);
        virtual void setUUID(QString, bool erase);

    private:

        QString filepath;
        TagLib::File * file;
        bool modified;
        double rating;
        QString keywords;
        QString uuid;
        QString duration;
};

#endif // MUSICFILE_HPP
