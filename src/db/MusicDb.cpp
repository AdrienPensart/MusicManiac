#include "MusicDb.hpp"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QDir>
#include <QVariant>

MusicDb::MusicDb(QObject * parent)	:
	db(QSqlDatabase::addDatabase("QSQLITE")),
	QObject(parent)
{
	auto dbpath = QDir::homePath()+"/music.db";
	QFile::remove(dbpath);
	db.setDatabaseName(dbpath);
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

void MusicDb::generateBest(){
	QSqlQuery artist_query(db);
	artist_query.exec(SELECT_ARTISTS);
	while(artist_query.next()){
		QString artist = artist_query.value(0).toString();
		QSqlQuery best(db);
		best.prepare(
		R"(
SELECT
	g.name AS `genre`,
	a.name AS `artist`,
	GROUP_CONCAT(t.name) AS `tags`,
	m.filepath AS `filepath`,
	m.uuid AS `uuid`,
	m.rating AS `rating`,
	m.duration AS `duration`
FROM music_tag mt
INNER JOIN music m ON m.id = mt.music_id
INNER JOIN artist a ON m.artist_id = a.id
INNER JOIN tag t ON t.id = mt.tag_id
INNER JOIN genre g ON g.id = m.genre_id
WHERE a.name = :artist AND m.rating >= 4 AND t.name != "cutoff"
GROUP BY m.id;
		)");
		best.bindValue(":artist", artist);
		best.exec();
		while (best.next()){
			for(int i = 0; i < best.record().count(); i++){
				qDebug() << best.value(i).toString();
			}
		}
	}
}

void MusicDb::generateBestByKeyword(){
	QSqlQuery artist_query(db);
	artist_query.exec(SELECT_ARTISTS);
	while(artist_query.next()){
		auto artist = artist_query.value(0);
		QSqlQuery tag_query(db);
		tag_query.exec(SELECT_TAGS);
		while(tag_query.next()){
			auto tag = tag_query.value(0);
			QSqlQuery best(db);
			best.prepare(
R"(
SELECT
	g.name AS `genre`,
	a.name AS `artist`,
	GROUP_CONCAT(t.name) AS `tags`,
	m.filepath AS `filepath`,
	m.uuid AS `uuid`,
	m.rating AS `rating`,
	m.duration AS `duration`
FROM music_tag mt
INNER JOIN music m ON m.id = mt.music_id
INNER JOIN artist a ON m.artist_id = a.id
INNER JOIN tag t ON t.id = mt.tag_id
INNER JOIN genre g ON g.id = m.genre_id
WHERE
	a.name = :artist AND
	m.rating >= 4 AND
	t.name != "cutoff"
GROUP BY m.id
HAVING GROUP_CONCAT(t.name) LIKE '%:tag%';
)");
			best.bindValue(":artist", artist);
			best.bindValue(":tag", tag);
			best.exec();
			while (best.next()){
				for(int i = 0; i < best.record().count(); i++){
					qDebug() << best.value(i).toString();
				}
			}
		}
	}
}

QString SELECT_TAGS = "SELECT name FROM tag;";
QString SELECT_ARTISTS = "SELECT name FROM artist;";
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
		   PRIMARY KEY (`music_id`, `tag_id`));)";
//		   FOREIGN KEY(`music_id`) REFERENCES music(id) ON DELETE CASCADE,
//		   FOREIGN KEY(`tag_id`) REFERENCES tag(id) ON DELETE CASCADE);)";

QString CREATE_MUSIC_INDEX = "CREATE INDEX IF NOT EXISTS `musicindex` ON music_tag(music_id);";
QString CREATE_TAG_INDEX = "CREATE INDEX IF NOT EXISTS `tagindex` ON music_tag(tag_id);";
