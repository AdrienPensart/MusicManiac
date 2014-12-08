#include "MainWindow.hpp"
#include <QApplication>
#include <log/Log.hpp>

int main(int argc, char *argv[])
{
    LOG.addObserver(new Log::LogToConsole("musicman", false));
    LOG << "test";
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
