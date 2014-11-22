#include "MusicDebugger.hpp"

#include <iostream>

MusicDebugger::MusicDebugger(){
    TagLib::setDebugListener(this);
}

MusicDebugger::~MusicDebugger(){
}

void MusicDebugger::setCurrentMusic(std::string current){
    currentMusic = current;
}

void MusicDebugger::printMessage (const TagLib::String &msg){
    std::cout << currentMusic << " : " << msg.toCString() << std::endl;
}
