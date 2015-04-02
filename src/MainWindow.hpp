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
    void refreshPlaylist();
    void selectArtist();
    void deselectArtist();
    void loadPlaylist(QModelIndex);

private:

    void updateFilter();
    void selectionToModel(QItemSelectionModel *, QStringListModel&, QStringListModel&);
    void loadFolderWith(bool regen);
    MusicFolderModel * musicModel;
    CustomSortFilterProxyModel * musicProxyModel;

    QItemSelectionModel * withoutKeywordsSelection;
    QStringListModel withoutKeywordsModel;

    QItemSelectionModel * availableKeywordsSelection;
    QStringListModel availableKeywordsModel;

    QItemSelectionModel * withKeywordsSelection;
    QStringListModel withKeywordsModel;

    QItemSelectionModel * availableArtistsSelection;
    QStringListModel availableArtistsModel;

    QItemSelectionModel * selectedArtistsSelection;
    QStringListModel selectedArtistsModel;

    QStringListModel playlistModel;
    Ui::MainWindow * ui;
    QString basefolder;
};
