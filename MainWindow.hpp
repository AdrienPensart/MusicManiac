#pragma once

#include <QMainWindow>
#include <QStringListModel>
#include <QItemSelectionModel>

#include "CustomSortFilterProxyModel.hpp"

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

    void generatePlaylist();
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
    CustomSortFilterProxyModel * musicProxyModel;

    QStringListModel withoutKeywordsModel;
    QStringListModel availableKeywordsModel;
    QStringListModel withKeywordsModel;

    QItemSelectionModel * withoutKeywordsSelection;
    QItemSelectionModel * availableKeywordsSelection;
    QItemSelectionModel * withKeywordsSelection;

    Ui::MainWindow * ui;
    QString basefolder;
};
