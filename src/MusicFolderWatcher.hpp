#pragma once

#include <QThread>
#include "inotify/Inotify.h"

class MusicFolderWatcher : public QThread
{
		Q_OBJECT
	public:
		explicit MusicFolderWatcher(QObject *parent = 0);

	protected:
		void run();

};
