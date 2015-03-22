#pragma once

#include <string>
#include <taglib/tdebuglistener.h>
#include <common/Singleton.hpp>

class MusicDebugger : public Common::Singleton<MusicDebugger>, TagLib::DebugListener
{
    friend class Common::Singleton<MusicDebugger>;

    public:

        void setCurrentMusic(std::string current);
        virtual void printMessage (const TagLib::String &msg);

    protected:

        MusicDebugger();
        virtual ~MusicDebugger();

    private:

        std::string currentMusic;
};
