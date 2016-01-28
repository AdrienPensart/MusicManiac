#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDebug>

#include "ui_mainwindow.h"

#include "common/Utility.hpp"
#include "MainWindow.hpp"
#include "Collection.hpp"
#include "PlaylistModel.hpp"
#include "PlaylistsModel.hpp"
#include "AlbumsModel.hpp"
#include "AlbumModel.hpp"
#include "ArtistModel.hpp"
#include "MusicModel.hpp"
#include "HorizontalProxyModel.hpp"

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

template<typename... Args> struct SELECT {
    template<typename C, typename R>
    static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
        return pmf;
    }
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setWindowIcon(QIcon(":/musicmaniac.ico"));
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::aboutQt);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionReset, &QAction::triggered, this, &MainWindow::reset);
    connect(ui->actionOpenFolder, &QAction::triggered, this, &MainWindow::loadFolder);
    connect(ui->actionOpenRegenFolder, &QAction::triggered, this, &MainWindow::loadFolderWithRegen);
    connect(ui->actionRescanFolder, &QAction::triggered, this, &MainWindow::rescanFolder);
    connect(ui->inWithButton, &QPushButton::clicked, this, &MainWindow::availableToWith);
    connect(ui->outWithButton, &QPushButton::clicked, this, &MainWindow::withToAvailable);
    connect(ui->inWithoutButton, &QPushButton::clicked, this, &MainWindow::availableToWithout);
    connect(ui->outWithoutButton, &QPushButton::clicked, this, &MainWindow::withoutToAvailable);
    connect(ui->inArtistButton, &QPushButton::clicked, this, &MainWindow::selectArtist);
    connect(ui->outArtistButton, &QPushButton::clicked, this, &MainWindow::deselectArtist);
    connect(ui->inGenreButton, &QPushButton::clicked, this, &MainWindow::selectGenre);
    connect(ui->outGenreButton, &QPushButton::clicked, this, &MainWindow::deselectGenre);
    connect(ui->collectionView, &QTreeView::clicked, this, &MainWindow::loadItem);

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

    connect(ui->ratingSpinBox, SELECT<double>::OVERLOAD_OF(&QDoubleSpinBox::valueChanged), this, &MainWindow::ratingChanged);
    connect(ui->minDurationEdit, &QLineEdit::textChanged, this, &MainWindow::minDurationChanged);
    connect(ui->maxDurationEdit, &QLineEdit::textChanged, this, &MainWindow::maxDurationChanged);

    playlistsModel = new PlaylistsModel(this);
    playlistModel = new PlaylistModel(this);
    albumsModel = new AlbumsModel(this);
    albumModel = new AlbumModel(this);
    artistModel = new ArtistModel(this);
    musicModel = new MusicModel(this);
    horizontalProxyModel = new HorizontalProxyModel(this);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::ratingChanged(double){

}

void MainWindow::minDurationChanged(QString minDuration){

}

void MainWindow::maxDurationChanged(QString maxDuration){

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
    if(type == "artist"){
        auto artistName = item->text();
        auto playlistsByArtist = collection.getPlaylistsByArtist();
        auto musicsByArtists = collection.getMusicsByArtists();
        auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();

        auto musics = musicsByArtists[artistName.toStdString()];
        auto playlists = playlistsByArtist[artistName.toStdString()];
        auto albums = musicsByArtistsAlbums[artistName.toStdString()];

        artistModel->set(artistName, musics, albums, playlists);
        horizontalProxyModel->setSourceModel(artistModel);
        ui->multiView->setModel(horizontalProxyModel);
        ui->multiView->reset();
    } else if(type == "playlists") {
        auto artistIndex = index.parent();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);
        auto artistName = artistItem->text();
        auto playlistsByArtist = collection.getPlaylistsByArtist();
        auto playlists = playlistsByArtist[artistName.toStdString()];
        ui->multiView->setModel(playlistsModel);
        playlistsModel->set(playlists);
    } else if(type == "playlist"){
        auto artistIndex = index.parent().parent();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);
        auto artistName = artistItem->text();
        auto playlists = collection.getPlaylistsByArtist();
        auto playlistName = item->data(Qt::DisplayRole).toString();
        qDebug() << "Artist = " << artistName << " and playlist = " << playlistName;
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
                ui->multiView->setModel(playlistModel);
                playlistModel->set(playlist);
                return;
            }
        }
    } else if(type == "albums"){
        auto artistIndex = index.parent();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);
        auto artistName = artistItem->text();
        auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();
        auto albums = musicsByArtistsAlbums[artistName.toStdString()];
        ui->multiView->setModel(albumsModel);
        albumsModel->set(albums);
    } else if(type == "album"){
        auto artistIndex = index.parent().parent();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);
        auto artistName = artistItem->text();
        auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();
        auto albumName = item->data(Qt::DisplayRole).toString();
        auto musics = musicsByArtistsAlbums[artistName.toStdString()][albumName.toStdString()];
        ui->multiView->setModel(albumModel);
        albumModel->set(musics);
    } else if(type == "song"){
        auto albumIndex = index.parent();
        auto albumItem = collectionModel.itemFromIndex(albumIndex);
        auto albumName = albumItem->text();

        auto artistIndex = albumIndex.parent().parent();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);
        auto artistName = artistItem->text();

        auto musicName = item->data(Qt::DisplayRole).toString();
        auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();
        auto music = musicsByArtistsAlbums[artistName.toStdString()][albumName.toStdString()][musicName.toStdString()];

        musicModel->set(music);
        horizontalProxyModel->setSourceModel(musicModel);
        ui->multiView->setModel(horizontalProxyModel);
        ui->multiView->reset();

    } else {
        qDebug() << "Type was not found : " << type;
    }
}

void MainWindow::reset() {
	withoutKeywordsModel.setStringList(empty);
	withKeywordsModel.setStringList(empty);
	availableArtistsModel.setStringList(empty);
	availableGenresModel.setStringList(empty);

    //selectedArtistsModel.setStringList(vectorToStringList(collection.getArtists()));
    //availableKeywordsModel.setStringList(vectorToStringList(collection.getKeywords()));
    //selectedGenresModel.setStringList(vectorToStringList(collection.getGenres()));

    //ui->musicView->resizeColumnsToContents();
    //ui->musicView->reset();
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
        qDebug() << "Invalid folder";
		return;
	}

    qDebug() << "Loading " << basefolder;
    collection.setRoot(basefolder.toStdString());
    collection.setRegen(regen);

	QProgressDialog progress("Loading your music...", "Abort", 0, collection.getTotalCount(), this);
	progress.setWindowTitle(this->windowTitle());
	progress.setWindowModality(Qt::WindowModal);
	progress.show();
	try {
		while(collection.factory()) {
			if (progress.wasCanceled()) {
                qDebug() << "Loading canceled";
				return;
			}
			progress.setValue(collection.getReadCount());
			QApplication::processEvents();
		}
	} catch (boost::filesystem::filesystem_error& fex) {
        qDebug() << "Exception " + QString(fex.what());
		progress.cancel();
	}
    progress.close();
    collection.refreshPlaylists();

    qDebug() << "Loading tree view";
    auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();
    auto playlistsByArtist = collection.getPlaylistsByArtist();
    collectionModel.clear();
    collectionModel.setHorizontalHeaderLabels(headerLabels);
    for(const auto& artist : musicsByArtistsAlbums){
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
            //QStandardItem * songHeaderItem = new QStandardItem("Song");
            //albumItem->appendRow(songHeaderItem);
            for(const auto& song : songs){
                QStandardItem * songItem = new QStandardItem(song.first.c_str());
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
