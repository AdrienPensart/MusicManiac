#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QItemSelectionModel>

#include "QCustomSortFilterProxyModel.hpp"

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
    void withoutToAvailable();
    void availableToWithout();
    void availableToWith();
    void withToAvailable();

private:

    void updateFilter();
    void selectionToModel(QItemSelectionModel *, QItemSelectionModel *, QStringListModel&, QStringListModel&);
    void loadFolderWith(bool regen);
    MusicFolderModel * musicModel;
    QCustomSortFilterProxyModel * musicProxyModel;

    QStringListModel withoutKeywordsModel;
    QStringListModel availableKeywordsModel;
    QStringListModel withKeywordsModel;

    QItemSelectionModel * withoutKeywordsSelection;
    QItemSelectionModel * availableKeywordsSelection;
    QItemSelectionModel * withKeywordsSelection;

    Ui::MainWindow * ui;
};

#endif // MAINWINDOW_H
