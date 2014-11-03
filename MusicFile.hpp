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
        virtual QString getDuration();
        virtual QString getFilepath();
        virtual double getRating();
        virtual QString getUUID();
        virtual QStringList getKeywords();
        virtual void setKeywords(const QStringList&)=0;

    protected:

        void setRating(double);
        void setUUID(QString);

    private:

        QStringList keywords;
        double rating;
        QString uuid;
        QString duration;
        QString filepath;
        TagLib::File * file;
};

#endif // MUSICFILE_HPP
