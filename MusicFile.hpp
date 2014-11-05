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
        virtual void setKeywords(QString);
        virtual void setRating(double);
        virtual void save();

    protected:

        void setUUID(QString);

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
