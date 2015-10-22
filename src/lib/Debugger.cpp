#include "Debugger.hpp"

Debugger::Debugger() {
	TagLib::setDebugListener(this);
}

Debugger::~Debugger() {
}

void Debugger::setCurrentMusic(std::string current) {
	currentMusic = current;
}

void Debugger::printMessage (const TagLib::String &msg) {
    std::cout << currentMusic << " : " << msg.toCString();
}
