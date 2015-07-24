#pragma once

#include <QDebug>
#include <QRunnable>
#include "lib/MusicFile.hpp"

std::string exec(std::string cmd);

class YoutubeFetcher : public QObject, public QRunnable {
	Q_OBJECT
	public:
		YoutubeFetcher(MusicFile * _mf) : mf(_mf){
		}

		void run(){
			auto search = "\"" + mf->getArtist() + " " + mf->getTitle() + "\"";
			auto cmd = "python3 /home/crunch/projects/musicmaniac/youtube_fetcher.py " + search;
			mf->setYoutube(exec(cmd));
			qDebug() << QString::fromStdString(search + " => " + mf->getYoutube());
		}

	private:
		MusicFile * mf;
};
