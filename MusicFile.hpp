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
        virtual double getRating()=0;
        virtual QString getUUID()=0;
        virtual void setKeywords(const QStringList&)=0;
        virtual QStringList getKeywords()=0;

    private:

        QString filepath;
        TagLib::File * file;
};

#endif // MUSICFILE_HPP
