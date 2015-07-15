#pragma once

#include <string>
#include <taglib/tdebuglistener.h>
#include <common/Singleton.hpp>

class Debugger : public Common::Singleton<Debugger>, TagLib::DebugListener {
		friend class Common::Singleton<Debugger>;

	public:

		void setCurrentMusic(std::string current);
		virtual void printMessage (const TagLib::String &msg);

	protected:

		Debugger();
		virtual ~Debugger();

	private:

		std::string currentMusic;
};
