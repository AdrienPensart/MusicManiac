#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

#include "ui_mainwindow.h"

#include "common/Utility.hpp"
#include "MainWindow.hpp"
#include "MusicFileFactory.hpp"
#include "MusicFolderModel.hpp"
#include "PlaylistModel.hpp"

QStringList vectorToStringList(const std::vector<std::string> input) {
	QStringList output;
	for(std::vector<std::string>::const_iterator iter = input.begin(); iter != input.end(); iter++) {
		output.append(iter->c_str());
	}
	return output;
}

std::vector<std::string> stringListToVector(const QStringList& input) {
	std::vector<std::string> output;
	foreach(QString str, input) {
		output.push_back(str.toStdString());
	}
	return output;
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);

	setWindowIcon(QIcon(":/musicmaniac.ico"));
	connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionOpenFolder, SIGNAL(triggered()), this, SLOT(loadFolder()));
	connect(ui->actionOpenRegenFolder, SIGNAL(triggered()), this, SLOT(loadFolderWithRegen()));
	connect(ui->inWithButton, SIGNAL(clicked()), this, SLOT(availableToWith()));
	connect(ui->outWithButton, SIGNAL(clicked()), this, SLOT(withToAvailable()));
	connect(ui->inWithoutButton, SIGNAL(clicked()), this, SLOT(availableToWithout()));
	connect(ui->outWithoutButton, SIGNAL(clicked()), this, SLOT(withoutToAvailable()));
	connect(ui->inArtistButton, SIGNAL(clicked()), this, SLOT(selectArtist()));
	connect(ui->outArtistButton, SIGNAL(clicked()), this, SLOT(deselectArtist()));
	connect(ui->playlistView, SIGNAL(clicked(QModelIndex)), this, SLOT(loadPlaylist(QModelIndex)));
	connect(ui->inGenreButton, SIGNAL(clicked()), this, SLOT(selectGenre()));
	connect(ui->outGenreButton, SIGNAL(clicked()), this, SLOT(deselectGenre()));
	connect(ui->actionGenerate, SIGNAL(triggered()), this, SLOT(generatePlaylist()));
	connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(reset()));

	musicProxyModel = new CustomSortFilterProxyModel(selectedArtistsModel, withoutKeywordsModel, withKeywordsModel, this);

	connect(ui->ratingSpinBox, SIGNAL(valueChanged(double)), musicProxyModel, SLOT(ratingChanged(double)));
	connect(ui->minDurationEdit, SIGNAL(textChanged(QString)), musicProxyModel, SLOT(minDurationChanged(QString)));
	connect(ui->maxDurationEdit, SIGNAL(textChanged(QString)), musicProxyModel, SLOT(maxDurationChanged(QString)));

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

	playlistModel = new PlaylistModel(this);
	musicModel = new MusicFolderModel(this);
	musicProxyModel->setSourceModel(musicModel);
	ui->playlistView->setModel(playlistModel);
	ui->musicView->setModel(musicProxyModel);
	ui->musicView->setSortingEnabled(true);
	ui->musicView->sortByColumn(MusicFolderModel::COLUMN_ARTIST, Qt::AscendingOrder);
	ui->musicView->horizontalHeader()->setStretchLastSection(true);
	musicProxyModel->setFilterKeyColumn(MusicFolderModel::COLUMN_KEYWORDS);
	musicProxyModel->setDynamicSortFilter(true);
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
	LOG << "Final path : " + finalPath.toStdString();
	Playlist playlist(finalPath.toStdString());

	playlist.setRating(ui->ratingSpinBox->value());
	playlist.setMaxDuration(ui->maxDurationEdit->text().toStdString());
	playlist.setMinDuration(ui->minDurationEdit->text().toStdString());
	playlist.setArtists(stringListToVector(selectedArtistsModel.stringList()));
	playlist.setWith(stringListToVector(withKeywordsModel.stringList()));
	playlist.setWithout(stringListToVector(withoutKeywordsModel.stringList()));

	for(int i = 0; i < musicProxyModel->rowCount(); i++) {
		QModelIndex index = musicProxyModel->index(i,0);
		QModelIndex index2 = musicProxyModel->mapToSource(index);
		playlist.add(musicModel->musicAt(index2.row()));
	}
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
	musicProxyModel->refilter();
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

void MainWindow::loadPlaylist(QModelIndex index) {
	QString playlistFilepath = playlistModel->itemData(index).first().toString();
	LOG << playlistFilepath.toStdString();
	Playlist playlist(playlistFilepath.toStdString());
	playlist.load();
	ui->ratingSpinBox->setValue(playlist.getRating());
	ui->maxDurationEdit->setText(playlist.getMaxDuration().c_str());
	ui->minDurationEdit->setText(playlist.getMinDuration().c_str());

	availableKeywordsModel.setStringList(empty);
	availableArtistsModel.setStringList(empty);
	selectedArtistsModel.setStringList(vectorToStringList(playlist.getArtists()));
	withKeywordsModel.setStringList(vectorToStringList(playlist.getWith()));
	withoutKeywordsModel.setStringList(vectorToStringList(playlist.getWithout()));
	musicProxyModel->refilter();
}

void MainWindow::reset() {
	withoutKeywordsModel.setStringList(empty);
	withKeywordsModel.setStringList(empty);
	availableArtistsModel.setStringList(empty);

	selectedArtistsModel.setStringList(musicModel->getArtists());
	availableKeywordsModel.setStringList(musicModel->getKeywords());
	ui->musicView->resizeColumnsToContents();
	ui->musicView->reset();
	musicProxyModel->refilter();
}

void MainWindow::loadFolder() {
	loadFolderWith(false);
}

void MainWindow::loadFolderWithRegen() {
	loadFolderWith(true);
}

void MainWindow::loadFolderWith(bool regen) {
	basefolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(!basefolder.size()) {
		LOG << "Invalid folder";
		return;
	}

	musicModel->clear();
	MusicFileFactory mff(basefolder.toStdString(), regen);
	QProgressDialog progress("Loading your music...", "Abort", 0, mff.getTotalCount(), this);
	progress.setWindowModality(Qt::WindowModal);

	ui->musicView->setUpdatesEnabled(false);
	try {
		while(mff.valid()) {
			if (progress.wasCanceled()) {
				break;
			}
			MusicFile * mf = mff.factory();
			if(mf) {
				musicModel->add(mf);
			}
			progress.setValue(mff.getReadCount());
		}
	} catch (boost::filesystem::filesystem_error& fex) {
		LOG << "Exception " + std::string(fex.what());
		progress.cancel();
	}
	ui->musicView->setUpdatesEnabled(true);

	if (!progress.wasCanceled()) {
		std::vector<Playlist *> playlists = mff.getPlaylists();
		for(std::vector<Playlist*>::iterator playlist = playlists.begin(); playlist != playlists.end(); playlist++) {
			(*playlist)->load();
			(*playlist)->refresh(musicModel->getMusics());
			playlistModel->add(*playlist);
		}
	}
	reset();
}

void MainWindow::aboutQt() {
	QMessageBox::aboutQt(this);
}

void MainWindow::about() {
	QMessageBox::about(this, tr("About MusicManiac"), tr("MusicManiac Project, made by Adrien Pensart"));
}
