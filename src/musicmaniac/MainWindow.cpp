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

const QString GLOBAL_MANUAL_PLAYLISTS = "global_manual_playlists";
const QString GLOBAL_AUTO_PLAYLISTS = "global_auto_playlists";
const QString ARTIST_MANUAL_PLAYLISTS = "artist_manual_playlists";
const QString ARTIST_AUTO_PLAYLISTS = "artist_auto_playlists";

const QString ARTIST = "artist";
const QString ALBUM = "album";
const QString ALBUMS = "albums";
const QString SONG = "song";
const QString SONGS = "songs";
const QString PLAYLIST = "playlist";

template<class Container>
QStringList toStringList(const Container& input) {
	QStringList output;
    for(const auto& str : input) {
        output.append(str.c_str());
	}
	return output;
}

template<class Container>
Container fromStringList(const QStringList& input) {
    Container output;
    for(const auto& str : input) {
        output.insert(output.end(), str.toStdString());
	}
	return output;
}

enum CollectionRoles {
    ItemTypeRole = Qt::UserRole + 1,
    ItemKey
};

template<typename... Args> struct SELECT {
    template<typename C, typename R>
    static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
        return pmf;
    }
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::MainWindow),
    playlist(0){
	ui->setupUi(this);
	setWindowIcon(QIcon(":/musicmaniac.ico"));
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::aboutQt);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionOpenFolder, &QAction::triggered, this, &MainWindow::loadFolder);
    connect(ui->actionOpenRegenFolder, &QAction::triggered, this, &MainWindow::loadFolderWithRegen);
    connect(ui->actionRescanFolder, &QAction::triggered, this, &MainWindow::rescanFolder);
    connect(ui->actionNewPlaylist, &QAction::triggered, this, &MainWindow::newPlaylist);
    connect(ui->collectionView, &QTreeView::clicked, this, &MainWindow::loadItem);

    connect(ui->inWithButton, &QPushButton::clicked, this,
            [=](){selectionToModel(availableKeywordsSelection, availableKeywordsModel, withKeywordsModel);});
    connect(ui->outWithButton, &QPushButton::clicked, this,
            [=](){selectionToModel(withKeywordsSelection, withKeywordsModel, availableKeywordsModel);});
    connect(ui->inWithoutButton, &QPushButton::clicked, this,
            [=](){selectionToModel(availableKeywordsSelection, availableKeywordsModel, withoutKeywordsModel);});
    connect(ui->outWithoutButton, &QPushButton::clicked, this,
            [=](){selectionToModel(withoutKeywordsSelection, withoutKeywordsModel, availableKeywordsModel);});
    connect(ui->inArtistButton, &QPushButton::clicked, this,
            [=](){selectionToModel(availableArtistsSelection, availableArtistsModel, selectedArtistsModel);});
    connect(ui->outArtistButton, &QPushButton::clicked, this,
            [=](){selectionToModel(selectedArtistsSelection, selectedArtistsModel, availableArtistsModel);});
    connect(ui->inGenreButton, &QPushButton::clicked, this,
            [=](){selectionToModel(availableGenresSelection, availableGenresModel, selectedGenresModel);});
    connect(ui->outGenreButton, &QPushButton::clicked, this,
            [=](){selectionToModel(selectedGenresSelection, selectedGenresModel, availableGenresModel);});


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

    connect(ui->ratingSpinBox, SELECT<double>::OVERLOAD_OF(&QDoubleSpinBox::valueChanged), this, &MainWindow::updatePlaylist);
    connect(ui->minDurationEdit, &QLineEdit::textChanged, this, &MainWindow::updatePlaylist);
    connect(ui->maxDurationEdit, &QLineEdit::textChanged, this, &MainWindow::updatePlaylist);

    ui->playlistSettingsBox->setVisible(false);
    ui->multiView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->actionRescanFolder->setEnabled(false);
    ui->actionNewPlaylist->setEnabled(false);

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

void MainWindow::newPlaylist(){
    if(!basefolder.size()) {
        QMessageBox::warning(this, tr("MusicMan"), tr("You did not load a folder yet."));
        return;
    }

    if(playlist){
        return;
    }

    playlist = new Playlist();
    collection.refreshPlaylist(playlist);
    ui->multiView->setModel(playlistModel);
    loadPlaylist(playlist);
}

bool MainWindow::savePlaylist(){
	if(!basefolder.size()) {
		QMessageBox::warning(this, tr("MusicMan"), tr("You did not load a folder yet."));
        return false;
	}

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Saving", "Do you want to save playlist ?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    switch(reply) {
        case QMessageBox::No:
            delete playlist;
            playlist = 0;
            qDebug() << "No pressed";
            return true;
            break;
        case QMessageBox::Cancel:
            qDebug() << "Cancel pressed";
            return false;
            break;
        case QMessageBox::Yes:
            qDebug() << "Yes pressed";
            break;
        default:
            break;
    }

    auto filename = (selectedArtistsModel.stringList()+withKeywordsModel.stringList()).join('_');
    QString filePath =  + "//" + filename;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Playlist"), filePath, tr("Playlist (*.m3u)"));
	if(!fileName.size()) {
        qDebug() << "Invalid filename";
        return false;
	}

    qDebug() << "Saving playlist to " << fileName;
    playlist->setFilepath(fileName.toStdString());
    playlist->save();
    collection.addPlaylist(playlist);
    playlist = 0;
    return true;
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
    updatePlaylist();
}

void MainWindow::updatePlaylist(){
    qDebug() << "Update playlist triggered.";
    auto playlist = playlistModel->getPlaylist();
    if(!playlist){
        qDebug() << "Invalid playlist.";
        return;
    }

    playlist->setRating(ui->ratingSpinBox->value());
    playlist->setMinDuration(ui->minDurationEdit->text().toStdString());
    playlist->setMaxDuration(ui->maxDurationEdit->text().toStdString());
    playlist->setArtists(fromStringList<std::set<std::string>>(selectedArtistsModel.stringList()));
    playlist->setGenres(fromStringList<std::set<std::string>>(selectedGenresModel.stringList()));
    playlist->setWith(fromStringList<std::set<std::string>>(withKeywordsModel.stringList()));
    playlist->setWithout(fromStringList<std::set<std::string>>(withoutKeywordsModel.stringList()));
    collection.refreshPlaylist(playlist);
    playlistModel->set(playlist);
}

void MainWindow::loadItem(QModelIndex index){
    if( (playlist && !savePlaylist()) || playlist){
        qDebug() << "Playlist not saved, not loading any item";
        return;
    }

    ui->playlistSettingsBox->setEnabled(false);

    auto item = collectionModel.itemFromIndex(index);
    auto type = item->data(CollectionRoles::ItemTypeRole).toString();
    qDebug() << "Loading item type" << type;
    if(type == ARTIST){
        ui->playlistSettingsBox->setVisible(false);
        auto artistName = item->text();
        auto manualPlaylistsByArtist = collection.getManualPlaylistsByArtist();
        auto autoPlaylistsByArtist = collection.getAutoPlaylistsByArtist();
        auto musicsByArtists = collection.getMusicsByArtists();
        auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();

        auto musics = musicsByArtists[artistName.toStdString()];
        auto manualPlaylists = manualPlaylistsByArtist[artistName.toStdString()];
        auto autoPlaylists = autoPlaylistsByArtist[artistName.toStdString()];
        auto albums = musicsByArtistsAlbums[artistName.toStdString()];

        artistModel->set(artistName, musics, albums, manualPlaylists, autoPlaylists);
        horizontalProxyModel->setSourceModel(artistModel);
        ui->multiView->setModel(horizontalProxyModel);
        ui->multiView->reset();
    } else if(type == GLOBAL_MANUAL_PLAYLISTS){
        ui->playlistSettingsBox->setVisible(false);
        auto playlists = collection.getManualPlaylists();
        ui->multiView->setModel(playlistsModel);
        playlistsModel->set(playlists);
    } else if(type == GLOBAL_AUTO_PLAYLISTS){
        ui->playlistSettingsBox->setVisible(false);
        auto playlists = collection.getAutogenPlaylists();
        ui->multiView->setModel(playlistsModel);
        playlistsModel->set(playlists);
    } else if(type == ARTIST_MANUAL_PLAYLISTS) {
        ui->playlistSettingsBox->setVisible(false);
        auto artistKey = item->data(CollectionRoles::ItemKey).toString().toStdString();
        auto playlistsByArtist = collection.getManualPlaylistsByArtist();
        if(playlistsByArtist.count(artistKey)){
            auto playlists = playlistsByArtist[artistKey];
            ui->multiView->setModel(playlistsModel);
            playlistsModel->set(playlists);
        }
    } else if(type == ARTIST_AUTO_PLAYLISTS){
        ui->playlistSettingsBox->setVisible(false);
        auto artistKey = item->data(CollectionRoles::ItemKey).toString().toStdString();
        auto playlistsByArtist = collection.getAutoPlaylistsByArtist();
        if(playlistsByArtist.count(artistKey)){
            auto playlists = playlistsByArtist[artistKey];
            ui->multiView->setModel(playlistsModel);
            playlistsModel->set(playlists);
        }
    } else if(type == PLAYLIST){
        auto playlistKey = item->data(CollectionRoles::ItemKey).toString().toStdString();
        auto playlists = collection.getAllPlaylists();
        if(playlists.count(playlistKey)){
            auto playlist = playlists[playlistKey];
            loadPlaylist(playlist);
        } else {
            qDebug() << playlistKey.c_str() << " not found";
        }
    } else if(type == ALBUMS){
        ui->playlistSettingsBox->setVisible(false);
        auto artistIndex = index.parent();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);
        auto artistName = artistItem->text();
        auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();
        auto albums = musicsByArtistsAlbums[artistName.toStdString()];
        ui->multiView->setModel(albumsModel);
        albumsModel->set(albums);
    } else if(type == ALBUM){
        ui->playlistSettingsBox->setVisible(false);
        auto artistIndex = index.parent().parent();
        auto artistItem = collectionModel.itemFromIndex(artistIndex);
        auto artistName = artistItem->text();
        auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();
        auto albumName = item->data(Qt::DisplayRole).toString();
        auto musics = musicsByArtistsAlbums[artistName.toStdString()][albumName.toStdString()];
        ui->multiView->setModel(albumModel);
        albumModel->set(musics);
    } else if(type == SONGS){
        auto artistKey = item->data(CollectionRoles::ItemKey).toString().toStdString();
        auto artists = collection.getMusicsByArtists();
        if(artists.count(artistKey)){
            auto musics = artists[artistKey];
            ui->multiView->setModel(albumModel);
            albumModel->set(musics);
        }
    } else if(type == SONG){
        auto musicKey = item->data(CollectionRoles::ItemKey).toString().toStdString();
        auto musics = collection.getMusics();
        if(musics.count(musicKey)){
            qDebug() << "Loading " << musicKey.c_str();
            auto music = musics[musicKey];
            musicModel->set(music);
            horizontalProxyModel->setSourceModel(musicModel);
            ui->multiView->setModel(horizontalProxyModel);
            ui->multiView->reset();
        } else {
            qDebug() << musicKey.c_str() << " not found";
        }
    } else {
        qDebug() << "Type was not found : " << type;
    }
}

void MainWindow::loadPlaylist(Playlist * playlist){

    ui->ratingSpinBox->blockSignals(true);
    ui->maxDurationEdit->blockSignals(true);
    ui->minDurationEdit->blockSignals(true);

    //qDebug() << "Loading rating : " << playlist->getRating();
    ui->ratingSpinBox->setValue(playlist->getRating());
    //qDebug() << "Loading max duration : " << playlist->getMaxDuration().c_str();
    ui->maxDurationEdit->setText(playlist->getMaxDuration().c_str());
    //qDebug() << "Loading min duration : " << playlist->getMinDuration().c_str();
    ui->minDurationEdit->setText(playlist->getMinDuration().c_str());

    // fill artists lists
    auto artists = playlist->getArtists();
    auto allArtists = collection.getArtists();
    decltype(artists) diffArtists;
    std::set_difference(allArtists.begin(), allArtists.end(),
                        artists.begin(), artists.end(),
                        std::inserter(diffArtists, diffArtists.begin()));
    //qDebug() << "Setting available artists " << toStringList(diffArtists);
    availableArtistsModel.setStringList(toStringList(diffArtists));
    //qDebug() << "Setting selected artists " << toStringList(artists);
    selectedArtistsModel.setStringList(toStringList(artists));

    // fill genres lists
    auto genres = playlist->getGenres();
    auto allGenres = collection.getGenres();
    decltype(genres) diffGenres;
    std::set_difference(allGenres.begin(), allGenres.end(),
                        genres.begin(), genres.end(),
                        std::inserter(diffGenres, diffGenres.begin()));
    //qDebug() << "Setting available genres " << toStringList(diffGenres);
    availableGenresModel.setStringList(toStringList(diffGenres));
    //qDebug() << "Setting selected genres " << toStringList(genres);
    selectedGenresModel.setStringList(toStringList(genres));

    // fill keywords lists
    auto allKeywords = collection.getKeywords();
    auto withKeywords = playlist->getWith();
    auto withoutKeywords = playlist->getWithout();
    decltype(allKeywords) diffKeywords;
    decltype(allKeywords) usedKeywords;
    std::set_union(withKeywords.begin(), withKeywords.end(),
                   withoutKeywords.begin(), withoutKeywords.end(),
                   std::inserter(usedKeywords, usedKeywords.begin()));
    std::set_difference(allKeywords.begin(), allKeywords.end(),
                        usedKeywords.begin(), usedKeywords.end(),
                        std::inserter(diffKeywords, diffKeywords.begin()));

    //qDebug() << "Setting available keywords : " << toStringList(diffKeywords);
    availableKeywordsModel.setStringList(toStringList(diffKeywords));
    //qDebug() << "Setting with keywords : " << toStringList(withKeywords);
    withKeywordsModel.setStringList(toStringList(withKeywords));
    //qDebug() << "Setting without keywords : " << toStringList(withoutKeywords);
    withoutKeywordsModel.setStringList(toStringList(withoutKeywords));

    playlistModel->set(playlist);
    ui->multiView->setModel(playlistModel);
    ui->playlistSettingsBox->setVisible(true);
    if(playlist->isManual()){
        ui->playlistSettingsBox->setEnabled(true);
    } else {
        ui->playlistSettingsBox->setEnabled(false);
    }

    ui->ratingSpinBox->blockSignals(false);
    ui->maxDurationEdit->blockSignals(false);
    ui->minDurationEdit->blockSignals(false);
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

    collection.generatePlaylists();
    qDebug() << "Loading tree view";
    auto musicsByArtistsAlbums = collection.getMusicsByArtistsAlbums();
    auto manualPlaylistsByArtist = collection.getManualPlaylistsByArtist();
    auto autoPlaylistsByArtist = collection.getAutoPlaylistsByArtist();
    auto playlists = collection.getAllPlaylists();
    auto songs = collection.getMusicsByArtists();
    collectionModel.clear();
    collectionModel.setHorizontalHeaderLabels(headerLabels);
    auto manualPlaylistsHeadersItem = new QStandardItem("Playlists (manual)");
    manualPlaylistsHeadersItem->setData(GLOBAL_MANUAL_PLAYLISTS, CollectionRoles::ItemTypeRole);
    manualPlaylistsHeadersItem->setBackground(Qt::yellow);

    auto autogenPlaylistsHeadersItem = new QStandardItem("Playlists (auto)");
    autogenPlaylistsHeadersItem->setData(GLOBAL_AUTO_PLAYLISTS, CollectionRoles::ItemTypeRole);
    autogenPlaylistsHeadersItem->setBackground(Qt::yellow);

    for(const auto& playlist : playlists){
        if(playlist.second->getArtists().size() > 1){
            auto playlistItem = new QStandardItem(playlist.second->getFilename().c_str());
            playlistItem->setData("playlist", CollectionRoles::ItemTypeRole);
            playlistItem->setData(playlist.second->getFilepath().c_str(), CollectionRoles::ItemKey);
            if(playlist.second->isManual()){
                manualPlaylistsHeadersItem->appendRow(playlistItem);
            } else if (playlist.second->getType() == AUTOGEN){
                autogenPlaylistsHeadersItem->appendRow(playlistItem);
            }
        }
    }

    if(manualPlaylistsHeadersItem->hasChildren()){
        collectionModel.appendRow(manualPlaylistsHeadersItem);
    }
    if(autogenPlaylistsHeadersItem->hasChildren()){
        collectionModel.appendRow(autogenPlaylistsHeadersItem);
    }

    for(const auto& artist : musicsByArtistsAlbums){
        auto artistItem = new QStandardItem(artist.first.c_str());
        artistItem->setData(ARTIST, CollectionRoles::ItemTypeRole);
        auto albums = artist.second;
        auto albumHeadersItem = new QStandardItem("Albums");
        albumHeadersItem->setBackground(Qt::green);
        albumHeadersItem->setData(ALBUMS, CollectionRoles::ItemTypeRole);

        auto songsHeadersItem = new QStandardItem("All Songs");
        songsHeadersItem->setBackground(Qt::gray);
        songsHeadersItem->setData(SONGS, CollectionRoles::ItemTypeRole);
        songsHeadersItem->setData(artist.first.c_str(), CollectionRoles::ItemKey);

        for(const auto& song : songs[artist.first.c_str()]){
            auto songItem = new QStandardItem(song.second->getTitle().c_str());
            songItem->setData(SONG, CollectionRoles::ItemTypeRole);
            songItem->setData(song.first.c_str(), CollectionRoles::ItemKey);
            songsHeadersItem->appendRow(songItem);
        }

        artistItem->appendRow(songsHeadersItem);
        for(const auto& album : albums){
            auto albumItem = new QStandardItem(album.first.c_str());
            albumItem->setData(ALBUM, CollectionRoles::ItemTypeRole);
            auto songs = album.second;
            for(const auto& song : songs){
                auto songItem = new QStandardItem(song.first.c_str());
                songItem->setData(SONG, CollectionRoles::ItemTypeRole);
                songItem->setData(song.second->getFilepath().c_str(), CollectionRoles::ItemKey);
                albumItem->appendRow(songItem);
            }
            albumHeadersItem->appendRow(albumItem);
        }
        artistItem->appendRow(albumHeadersItem);

        if(manualPlaylistsByArtist.count(artist.first)){
            auto playlists = manualPlaylistsByArtist[artist.first];
            auto manualPlaylistsItem = new QStandardItem("Playlists (manual)");
            manualPlaylistsItem->setData(ARTIST_MANUAL_PLAYLISTS, CollectionRoles::ItemTypeRole);
            manualPlaylistsItem->setData(artist.first.c_str(), CollectionRoles::ItemKey);
            manualPlaylistsItem->setBackground(Qt::yellow);

            for(const auto& playlist : playlists){
                auto playlistItem = new QStandardItem(playlist.second->getFilename().c_str());
                playlistItem->setData(PLAYLIST, CollectionRoles::ItemTypeRole);
                playlistItem->setData(playlist.second->getFilepath().c_str(), CollectionRoles::ItemKey);
                manualPlaylistsItem->appendRow(playlistItem);
            }

            artistItem->appendRow(manualPlaylistsItem);
        }

        if(autoPlaylistsByArtist.count(artist.first)){
            auto playlists = autoPlaylistsByArtist[artist.first];
            auto autoPlaylistsItem = new QStandardItem("Playlists (auto)");
            autoPlaylistsItem->setData(ARTIST_AUTO_PLAYLISTS, CollectionRoles::ItemTypeRole);
            autoPlaylistsItem->setData(artist.first.c_str(), CollectionRoles::ItemKey);
            autoPlaylistsItem->setBackground(Qt::yellow);

            for(const auto& playlist : playlists){
                auto playlistItem = new QStandardItem(playlist.second->getFilename().c_str());
                playlistItem->setData(PLAYLIST, CollectionRoles::ItemTypeRole);
                playlistItem->setData(playlist.second->getFilepath().c_str(), CollectionRoles::ItemKey);
                autoPlaylistsItem->appendRow(playlistItem);
            }
            artistItem->appendRow(autoPlaylistsItem);
        }
        collectionModel.appendRow(artistItem);
    }
    ui->collectionView->setModel(&collectionModel);
    ui->actionRescanFolder->setEnabled(true);
    ui->actionNewPlaylist->setEnabled(true);
}

void MainWindow::aboutQt() {
	QMessageBox::aboutQt(this);
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About MusicManiac"), tr("MusicManiac Project, made by Adrien Pensart"));
}
