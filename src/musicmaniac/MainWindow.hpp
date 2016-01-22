#pragma once

#include <QMainWindow>
#include <QStringListModel>
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include "db/MusicDb.hpp"

namespace Ui {
	class MainWindow;
}

class MusicFile;
class PlaylistModel;
class PlaylistsModel;

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:

		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

    private slots:

		void about();
		void aboutQt();
		void generatePlaylist();
		void loadFolder();
		void loadFolderWithRegen();
		void withoutToAvailable();
		void availableToWithout();
		void availableToWith();
		void withToAvailable();
		void selectArtist();
		void deselectArtist();
		void selectGenre();
		void deselectGenre();
        void loadItem(QModelIndex);
		void reset();
		void rescanFolder(bool=false);
        void ratingChanged(double);
        void minDurationChanged(QString);
        void maxDurationChanged(QString);

	private:

		void updateFilter();
		void selectionToModel(QItemSelectionModel *, QStringListModel&, QStringListModel&);
		void loadFolderWith(bool regen);
        QStandardItemModel collectionModel;
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

        PlaylistsModel * playlistsModel;
        PlaylistModel * playlistModel;
		Ui::MainWindow * ui;
		QString basefolder;
        Collection collection;
        MusicDb db;
        QStringList headerLabels;
};
