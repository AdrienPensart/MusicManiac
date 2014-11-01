#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void chooseFolder();

private:

    void scanFolder(QDir dir);
    QString getDuration(TagLib::File * file);
    void addMP3(QString filepath);
    void addFLAC(QString filepath);
    void addRow(QString filepath, QString duration, double rating, QString uuid);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
