#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

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

    void loadFolder();
    void addMissingMP3UUID();
    void addMissingFLACUUID();

private:
    Ui::MainWindow *ui;
    void scanDirIter(QDir dir);
};

#endif // MAINWINDOW_H
