#pragma once

#include <QMainWindow>
#include <QStringListModel>
#include <QItemSelectionModel>
#include "db/MusicDb.hpp"

namespace Ui {
	class MainWindow;
}

class MusicFile;
class PlaylistModel;
class PlaylistsModel;
class AlbumsModel;
class AlbumModel;
class ArtistModel;
class MusicModel;
class CollectionModel;
class HorizontalProxyModel;

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:

		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

    private slots:

		void about();
		void aboutQt();

		void loadFolder();
		void loadFolderWithRegen();
        void loadItem(QModelIndex);
		void rescanFolder(bool=false);

        // playlist management
        void addToPlaylist(const QString& key);
        void newAutomaticPlaylist();
        void newManualPlaylist();
        void updatePlaylist();
        void discardPlaylist();
        void savePlaylist();
        void loadPlaylist(Playlist * playlist);

	private:

        void rebuild();
		void selectionToModel(QItemSelectionModel *, QStringListModel&, QStringListModel&);
		void loadFolderWith(bool regen);
        CollectionModel * collectionModel;
        //CustomSortFilterProxyModel * musicProxyModel;
		const QStringList empty;
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

		QItemSelectionModel * availableGenresSelection;
		QStringListModel availableGenresModel;

		QItemSelectionModel * selectedGenresSelection;
		QStringListModel selectedGenresModel;

        HorizontalProxyModel * horizontalProxyModel;
        PlaylistsModel * playlistsModel;
        PlaylistModel * playlistModel;
        AlbumsModel * albumsModel;
        AlbumModel * albumModel;
        ArtistModel * artistModel;
        MusicModel * musicModel;

		Ui::MainWindow * ui;
		QString basefolder;
        Collection collection;
        MusicDb db;
        QStringList headerLabels;
        Playlist * playlist;
};
