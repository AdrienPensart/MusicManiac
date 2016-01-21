#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDebug>

#include "ui_mainwindow.h"

#include "common/Utility.hpp"
#include "MainWindow.hpp"
#include "Collection.hpp"
#include "PlaylistModel.hpp"

using std::cout;

QStringList vectorToStringList(const std::vector<std::string>& input) {
	QStringList output;
	for(std::vector<std::string>::const_iterator iter = input.begin(); iter != input.end(); iter++) {
		output.append(iter->c_str());
	}
	return output;
}

std::vector<std::string> stringListToVector(const QStringList& input) {
	std::vector<std::string> output;
    for(auto str : input) {
		output.push_back(str.toStdString());
	}
	return output;
}

enum CollectionRoles {
    ItemTypeRole = Qt::UserRole + 1
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setWindowIcon(QIcon(":/musicmaniac.ico"));
	connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionOpenFolder, SIGNAL(triggered()), this, SLOT(loadFolder()));
	connect(ui->actionOpenRegenFolder, SIGNAL(triggered()), this, SLOT(loadFolderWithRegen()));
	connect(ui->actionRescanFolder, SIGNAL(triggered()), this, SLOT(rescanFolder()));
	connect(ui->inWithButton, SIGNAL(clicked()), this, SLOT(availableToWith()));
	connect(ui->outWithButton, SIGNAL(clicked()), this, SLOT(withToAvailable()));
	connect(ui->inWithoutButton, SIGNAL(clicked()), this, SLOT(availableToWithout()));
	connect(ui->outWithoutButton, SIGNAL(clicked()), this, SLOT(withoutToAvailable()));
	connect(ui->inArtistButton, SIGNAL(clicked()), this, SLOT(selectArtist()));
	connect(ui->outArtistButton, SIGNAL(clicked()), this, SLOT(deselectArtist()));
	connect(ui->inGenreButton, SIGNAL(clicked()), this, SLOT(selectGenre()));
	connect(ui->outGenreButton, SIGNAL(clicked()), this, SLOT(deselectGenre()));
	connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(reset()));
    connect(ui->collectionView, SIGNAL(clicked(QModelIndex)), this, SLOT(loadItem(QModelIndex)));

    headerLabels.append("Artist");
	withoutKeywordsSelection = new QItemSelectionModel(&withoutKeywordsModel);
	ui->withoutKeywordsView->setModel(&withoutKeywordsModel);
	ui->withoutKeywordsView->setSelectionModel(withoutKeywordsSelection);

	availableKeywordsSelection = new QItemSelectionModel(&availableKeywordsModel);
	ui->availableKeywordsView->setModel(&availableKeywordsModel);
	ui->availableKeywordsView->setSelectionModel(availableKeywordsSelection);

	withKeywordsSelection = new QItemSelectionModel(&withKeywordsModel);
	ui->withKeywordsView->setModel(&withKeywordsModel);
	ui->withKeywordsView->setSelectionModel(withKeywordsSelection);

	availableArtistsSelection = new QItemSelectionModel(&availableArtistsModel);
	ui->availableArtistsView->setModel(&availableArtistsModel);
	ui->availableArtistsView->setSelectionModel(availableArtistsSelection);

	selectedArtistsSelection = new QItemSelectionModel(&selectedArtistsModel);
	ui->selectedArtistsView->setModel(&selectedArtistsModel);
	ui->selectedArtistsView->setSelectionModel(selectedArtistsSelection);

	availableGenresSelection = new QItemSelectionModel(&availableGenresModel);
	ui->availableGenresView->setModel(&availableGenresModel);
	ui->availableGenresView->setSelectionModel(availableGenresSelection);

	selectedGenresSelection = new QItemSelectionModel(&selectedGenresModel);
	ui->selectedGenresView->setModel(&selectedGenresModel);
	ui->selectedGenresView->setSelectionModel(selectedGenresSelection);

    /*
    connect(ui->playlistView, SIGNAL(clicked(QModelIndex)), this, SLOT(loadPlaylist(QModelIndex)));
    musicProxyModel = new CustomSortFilterProxyModel(selectedGenresModel, selectedArtistsModel, withoutKeywordsModel, withKeywordsModel, this);
    connect(ui->ratingSpinBox, SIGNAL(valueChanged(double)), musicProxyModel, SLOT(ratingChanged(double)));
    connect(ui->minDurationEdit, SIGNAL(textChanged(QString)), musicProxyModel, SLOT(minDurationChanged(QString)));
    connect(ui->maxDurationEdit, SIGNAL(textChanged(QString)), musicProxyModel, SLOT(maxDurationChanged(QString)));
	playlistModel = new PlaylistModel(this);
    musicModel = new MusicModel(collection, this);
	musicProxyModel->setSourceModel(musicModel);
	ui->playlistView->setModel(playlistModel);
	ui->playlistView->setSortingEnabled(true);
	ui->playlistView->sortByColumn(PlaylistModel::COLUMN_ARTISTS, Qt::AscendingOrder);
	ui->playlistView->horizontalHeader()->setStretchLastSection(true);
    */

    //ui->musicView->setModel(musicProxyModel);
    //ui->musicView->setModel(&musicModel);
    /*
	ui->musicView->setSortingEnabled(true);
    ui->musicView->sortByColumn(MusicModel::COLUMN_ARTIST, Qt::AscendingOrder);
    ui->musicView->horizontalHeader()->setStretchLastSection(true);
    musicProxyModel->setFilterKeyColumn(MusicModel::COLUMN_KEYWORDS);
	musicProxyModel->setDynamicSortFilter(true);
    */
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::generatePlaylist() {
	if(!basefolder.size()) {
		QMessageBox::warning(this, tr("MusicMan"), tr("You did not load a folder yet."));
		return;
	}

	QString filename = withKeywordsModel.stringList().join('_');
	if(!filename.size()) {
		filename = "all";
	}
	QString filePath = basefolder + "//" + filename;
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Playlist"), filePath, tr("Playlist (*.m3u)"));
	if(!fileName.size()) {
		return;
	}
	QString finalPath = basefolder + "//" + fileName;
	// "Final path : " + finalPath.toStdString();
	Playlist playlist(finalPath.toStdString());

	playlist.setRating(ui->ratingSpinBox->value());
	playlist.setMaxDuration(ui->maxDurationEdit->text().toStdString());
	playlist.setMinDuration(ui->minDurationEdit->text().toStdString());
	playlist.setArtists(stringListToVector(selectedArtistsModel.stringList()));
	playlist.setWith(stringListToVector(withKeywordsModel.stringList()));
	playlist.setWithout(stringListToVector(withoutKeywordsModel.stringList()));

    /*
	for(int i = 0; i < musicProxyModel->rowCount(); i++) {
		QModelIndex index = musicProxyModel->index(i,0);
		QModelIndex index2 = musicProxyModel->mapToSource(index);
        playlist.add(musicModel->musicAt(index2.row()));
	}
    */
	playlist.save();
}

void MainWindow::selectionToModel(QItemSelectionModel * sourceSelection, QStringListModel& sourceModel, QStringListModel& destinationModel) {
	QModelIndexList indexes = sourceSelection->selection().indexes();
	QStringList list = destinationModel.stringList();
	while(indexes.size()) {
		list.append(sourceModel.data(indexes.first(), Qt::DisplayRole).toString());
		sourceModel.removeRow(indexes.first().row());
		indexes = sourceSelection->selection().indexes();
	}
	list.sort();
	destinationModel.setStringList(list);
    //musicProxyModel->refilter();
}

void MainWindow::selectGenre() {
	selectionToModel(availableGenresSelection, availableGenresModel, selectedGenresModel);
}

void MainWindow::deselectGenre() {
	selectionToModel(selectedGenresSelection, selectedGenresModel, availableGenresModel);
}

void MainWindow::selectArtist() {
	selectionToModel(availableArtistsSelection, availableArtistsModel, selectedArtistsModel);
}

void MainWindow::deselectArtist() {
	selectionToModel(selectedArtistsSelection, selectedArtistsModel, availableArtistsModel);
}

void MainWindow::withoutToAvailable() {
	selectionToModel(withoutKeywordsSelection, withoutKeywordsModel, availableKeywordsModel);
}

void MainWindow::availableToWithout() {
	selectionToModel(availableKeywordsSelection, availableKeywordsModel, withoutKeywordsModel);
}

void MainWindow::availableToWith() {
	selectionToModel(availableKeywordsSelection, availableKeywordsModel, withKeywordsModel);
}

void MainWindow::withToAvailable() {
	selectionToModel(withKeywordsSelection, withKeywordsModel, availableKeywordsModel);
}

void MainWindow::loadItem(QModelIndex index){
    auto item = collectionModel.itemFromIndex(index);
    auto type = item->data(CollectionRoles::ItemTypeRole).toString();
    if(type == "playlist"){
        auto artistIndex = index.parent().parent();
        auto playlists = collection.getPlaylistsByArtist();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);

        auto playlistName = item->data(Qt::DisplayRole).toString();
        auto artistName = artistItem->text();
        qDebug() << "Playlist " << playlistName << " of artist " << artistName;
        for(const auto& playlist : playlists[artistName.toStdString()]) {
            if(playlistName.toStdString() == playlist->getFilename()) {
                ui->ratingSpinBox->setValue(playlist->getRating());
                ui->maxDurationEdit->setText(playlist->getMaxDuration().c_str());
                ui->minDurationEdit->setText(playlist->getMinDuration().c_str());

                availableArtistsModel.setStringList(empty);
                selectedArtistsModel.setStringList(vectorToStringList(playlist->getArtists()));

                availableGenresModel.setStringList(empty);
                selectedGenresModel.setStringList(vectorToStringList(playlist->getGenres()));

                withKeywordsModel.setStringList(vectorToStringList(playlist->getWith()));
                withoutKeywordsModel.setStringList(vectorToStringList(playlist->getWithout()));

                //musicProxyModel->refilter();
                //availableKeywordsModel.setStringList(musicProxyModel->getKeywords());
                return;
            }
        }
        qDebug() << " was not found";
    }
}

void MainWindow::reset() {
    /*
	withoutKeywordsModel.setStringList(empty);
	withKeywordsModel.setStringList(empty);
	availableArtistsModel.setStringList(empty);
	availableGenresModel.setStringList(empty);

    selectedArtistsModel.setStringList(musicModel->getArtists());
    availableKeywordsModel.setStringList(musicModel->getKeywords());
    selectedGenresModel.setStringList(musicModel->getGenres());

    ui->musicView->resizeColumnsToContents();
    ui->musicView->reset();
    musicProxyModel->refilter();
    */
}

void MainWindow::loadFolder() {
	loadFolderWith(false);
}

void MainWindow::loadFolderWithRegen() {
	loadFolderWith(true);
}

void MainWindow::loadFolderWith(bool regen) {
	basefolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	rescanFolder(regen);
}

void MainWindow::rescanFolder(bool regen){
	if(!basefolder.size()) {
        cout << "Invalid folder\n";
		return;
	}

    cout << "Loading " << basefolder.toStdString() << "\n";
    collection.setRoot(basefolder.toStdString());
    collection.setRegen(regen);

	QProgressDialog progress("Loading your music...", "Abort", 0, collection.getTotalCount(), this);
	progress.setWindowTitle(this->windowTitle());
	progress.setWindowModality(Qt::WindowModal);
	progress.show();
	try {
		while(collection.factory()) {
			if (progress.wasCanceled()) {
                cout << "Loading canceled\n";
				return;
			}
			progress.setValue(collection.getReadCount());
			QApplication::processEvents();
		}
	} catch (boost::filesystem::filesystem_error& fex) {
        cout << "Exception " + std::string(fex.what());
		progress.cancel();
	}
    progress.close();

    cout << "Loading tree view\n";
    auto tree = collection.buildTree();
    auto playlistsByArtist = collection.getPlaylistsByArtist();
    collectionModel.clear();
    collectionModel.setHorizontalHeaderLabels(headerLabels);
    for(const auto& artist : tree){
        QStandardItem * artistItem = new QStandardItem(artist.first.c_str());
        artistItem->setData("artist", CollectionRoles::ItemTypeRole);
        auto albums = artist.second;
        QStandardItem * albumHeadersItem = new QStandardItem("Albums");
        albumHeadersItem->setBackground(Qt::green);
        albumHeadersItem->setData("albums", CollectionRoles::ItemTypeRole);
        for(const auto& album : albums){
            QStandardItem * albumItem = new QStandardItem(album.first.c_str());
            albumItem->setData("album", CollectionRoles::ItemTypeRole);
            auto songs = album.second;
            QStandardItem * songHeaderItem = new QStandardItem("Song");
            albumItem->appendRow(songHeaderItem);
            for(const auto& song : songs){
                QStandardItem * songItem = new QStandardItem(song->getTitle().c_str());
                songItem->setData("song", CollectionRoles::ItemTypeRole);
                albumItem->appendRow(songItem);
            }
            albumHeadersItem->appendRow(albumItem);
        }
        artistItem->appendRow(albumHeadersItem);

        auto playlists = playlistsByArtist[artist.first];
        if(playlists.size())
        {
            QStandardItem * playlistsItem = new QStandardItem("Playlists");
            playlistsItem->setData("playlists", CollectionRoles::ItemTypeRole);
            playlistsItem->setBackground(Qt::yellow);
            for(const auto& playlist : playlists){
                QStandardItem * playlistItem = new QStandardItem(playlist->getFilename().c_str());
                playlistItem->setData("playlist", CollectionRoles::ItemTypeRole);
                playlistsItem->appendRow(playlistItem);
            }
            artistItem->appendRow(playlistsItem);
        }
        collectionModel.appendRow(artistItem);
    }
    ui->collectionView->setModel(&collectionModel);
}

void MainWindow::aboutQt() {
	QMessageBox::aboutQt(this);
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About MusicManiac"), tr("MusicManiac Project, made by Adrien Pensart"));
}
