#include "MainWindow.hpp"
#include <QApplication>
#include <log/Log.hpp>

int main(int argc, char *argv[]) {
	LOG.addObserver(new Log::LogToConsole("MusicManiac", false));
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
