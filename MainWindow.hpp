#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MusicFile;
class MusicFolderModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void loadFolder();
    void loadFolderWithRegen();

private:

    void loadFolderWith(bool regen);
    MusicFolderModel * musicModel;
    Ui::MainWindow * ui;
};

#endif // MAINWINDOW_H
