#ifndef MUSICFOLDERWATCHER_HPP
#define MUSICFOLDERWATCHER_HPP

#include <QThread>

class MusicFolderWatcher : public QThread
{
		Q_OBJECT
	public:
		explicit MusicFolderWatcher(QObject *parent = 0);

	signals:

	public slots:

};

#endif // MUSICFOLDERWATCHER_HPP
