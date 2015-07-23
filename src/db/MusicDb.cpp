#include "MusicDb.hpp"
#include <QDebug>
#include <QDir>
#include <QVariant>

MusicDb::MusicDb(QObject * parent)	:
	db(QSqlDatabase::addDatabase("QSQLITE")),
	QObject(parent)
{
	db.setDatabaseName(QDir::homePath()+"/music.db");
	if(db.open()){
		QSqlDatabase::database().transaction();
		QSqlQuery query(db);
		query.exec(ENABLE_FOREIGN_KEYS);
		query.exec(CREATE_ARTIST_TABLE);
		query.exec(CREATE_GENRE_TABLE);
		query.exec(CREATE_MUSIC_TABLE);
		query.exec(CREATE_TAG_TABLE);
		query.exec(CREATE_MUSIC_TAG_TABLE);
		query.exec(CREATE_MUSIC_INDEX);
		query.exec(CREATE_TAG_INDEX);
		QSqlDatabase::database().commit();
	} else {
		qDebug() << "Can't open database";
	}
}

void MusicDb::save(Collection& collection){
	QSqlDatabase::database().transaction();
	auto musics = collection.getMusics();
	for(auto music : musics){
		save(music.second);
	}
	QSqlDatabase::database().commit();
}

void MusicDb::save(MusicFile * mf){
	QSqlQuery query(db);
	// insert the artist
	query.prepare("INSERT OR IGNORE INTO artist (name) VALUES(:name)");
	query.bindValue(":name", QVariant(mf->getArtist().c_str()));
	query.exec();

	// insert the genre
	query.prepare("INSERT OR IGNORE INTO genre (name) VALUES(:name)");
	query.bindValue(":name", QVariant(mf->getGenre().c_str()));
	query.exec();

	// insert music
	query.prepare("INSERT OR IGNORE INTO music (artist_id, genre_id, rating, duration, uuid, filepath)"
				  "VALUES((SELECT id FROM artist WHERE name = :artist), (SELECT id FROM genre WHERE name = :genre), :rating, :duration, :uuid, :filepath)");
	query.bindValue(":artist", QVariant(mf->getArtist().c_str()));
	query.bindValue(":genre", QVariant(mf->getGenre().c_str()));
	query.bindValue(":rating", QVariant(mf->getRating()));
	query.bindValue(":duration", QVariant(mf->getDuration().c_str()));
	query.bindValue(":uuid", QVariant(mf->getUUID().c_str()));
	query.bindValue(":filepath", QVariant(mf->getFilepath().c_str()));
	query.exec();

	// insert tags
	for(auto tag : mf->getSplittedKeywords()){
		query.prepare("INSERT OR IGNORE INTO tag (name) VALUES(:name)");
		query.bindValue(":name", QVariant(tag.c_str()));
		query.exec();

		query.prepare("INSERT OR IGNORE INTO music_tag (music_id, tag_id) VALUES ((SELECT id FROM music WHERE filepath = :filepath), (SELECT id FROM tag WHERE name = :tag))");
		query.bindValue(":filepath", QVariant(mf->getFilepath().c_str()));
		query.bindValue(":tag", QVariant(tag.c_str()));
		query.exec();
	}
}

QString ENABLE_FOREIGN_KEYS = "PRAGMA foreign_keys = ON;";
QString CREATE_ARTIST_TABLE = "CREATE TABLE IF NOT EXISTS `artist`(`id` INTEGER PRIMARY KEY, `name` VARCHAR UNIQUE);";
QString CREATE_GENRE_TABLE = "CREATE TABLE IF NOT EXISTS `genre` (`id` INTEGER PRIMARY KEY, `name` VARCHAR UNIQUE);";
QString CREATE_MUSIC_TABLE = R"(CREATE TABLE IF NOT EXISTS `music` (
		   `id` INTEGER PRIMARY KEY,
		   `artist_id` INTEGER,
		   `genre_id` INTEGER,
		   `rating` REAL,
		   `duration` VARCHAR(255),
		   `uuid` VARCHAR(255),
		   `filepath` VARCHAR UNIQUE,
		   FOREIGN KEY(`artist_id`) REFERENCES artist(id),
		   FOREIGN KEY(`genre_id`) REFERENCES genre(id));)";

QString CREATE_TAG_TABLE = "CREATE TABLE IF NOT EXISTS `tag`(`id` INTEGER PRIMARY KEY, `name` VARCHAR UNIQUE);";
QString CREATE_MUSIC_TAG_TABLE = R"(CREATE TABLE IF NOT EXISTS `music_tag`(
		   `music_id` INTEGER,
		   `tag_id` INTEGER,
		   FOREIGN KEY(`music_id`) REFERENCES music(id) ON DELETE CASCADE,
		   FOREIGN KEY(`tag_id`) REFERENCES tag(id) ON DELETE CASCADE);)";

QString CREATE_MUSIC_INDEX = "CREATE INDEX IF NOT EXISTS `musicindex` ON music_tag(music_id);";
QString CREATE_TAG_INDEX = "CREATE INDEX IF NOT EXISTS `tagindex` ON music_tag(tag_id);";
