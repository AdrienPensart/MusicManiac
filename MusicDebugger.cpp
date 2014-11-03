#include "MusicDebugger.hpp"

#include <QDebug>

MusicDebugger::MusicDebugger(){
    TagLib::setDebugListener(this);
}

MusicDebugger::~MusicDebugger(){
}

void MusicDebugger::setCurrentMusic(QString current){
    currentMusic = current;
}

void MusicDebugger::printMessage (const TagLib::String &msg){
    qDebug() << currentMusic << " : " << msg.toCString();
}
