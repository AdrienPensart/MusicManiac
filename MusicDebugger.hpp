#ifndef MUSICDEBUGGER_HPP
#define MUSICDEBUGGER_HPP

#include <QString>
#include <taglib/tdebuglistener.h>
#include "Singleton.hpp"

class MusicDebugger : public Common::Singleton<MusicDebugger>, TagLib::DebugListener
{
    friend class Common::Singleton<MusicDebugger>;

    public:

        void setCurrentMusic(QString current);
        virtual void printMessage (const TagLib::String &msg);

    protected:

        MusicDebugger();
        virtual ~MusicDebugger();

    private:

        QString currentMusic;
};

#endif // MUSICDEBUGGER_HPP
