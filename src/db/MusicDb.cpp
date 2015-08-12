#include "MusicDb.hpp"
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QDir>
#include <QVariant>
#include <QThreadPool>

void exec(QSqlQuery& query, const std::string& sql=""){
	bool result = false;
	if(sql == ""){
		result = query.exec();
	} else {
		result = query.exec(sql.c_str());
	}
	if(!result){
		throw MusicDbException(sql + " - can't execute : \n" + query.lastQuery().toStdString() + "\nreason\n" + query.lastError().text().toStdString());
	}
}

YoutubeFetcher::YoutubeFetcher(QSqlDatabase& _db, QVariant _id, QString _title, QString _album, QString _artist) :
	db(_db),
	id(_id),
	title(_title),
	album(_album),
	artist(_artist)
{}

void YoutubeFetcher::run(){
	/*
	// try with artist + title
	auto search = "\"" + artist + " " + title + "\"";
	auto cmd = "python3 /home/crunch/projects/musicmaniac/youtube_fetcher.py " + search;
	std::string youtube = execCmd(cmd.toStdString());

	// try with artist + album + title
	if(!youtube.size()){
	*/
		auto search = "\"" + artist + " " + album + " " + title + "\"";
		auto cmd = "python3 /home/crunch/projects/musicmaniac/youtube_fetcher.py " + search;
		std::string youtube = execCmd(cmd.toStdString());
	//}

	if(youtube == ""){
		youtube = "notfound";
	} else if(youtube == "error"){
		youtube = "";
		return;
	}

	QSqlQuery youtube_query(db);
	youtube_query.prepare("UPDATE music SET youtube = :youtube WHERE id = :id");
	youtube_query.bindValue(":youtube", youtube.c_str());
	youtube_query.bindValue(":id", id);
	exec(youtube_query);
	qDebug() << search + " => " + QString::fromStdString(youtube);
}

std::string YoutubeFetcher::execCmd(std::string cmd){
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe){
		abort();
	}
	char buffer[512];
	std::string result;
	while(!feof(pipe)) {
		if(fgets(buffer, sizeof(buffer), pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

void MusicDb::begin(){
	if(!QSqlDatabase::database().transaction()){
		throw Common::Exception("Can't begin transaction");
	}
}

void MusicDb::commit(){
	if(!QSqlDatabase::database().commit()){
		throw Common::Exception("Can't commit transaction");
	}
}

MusicDb::MusicDb(bool toDelete, QObject * parent):
	db(QSqlDatabase::addDatabase("QSQLITE")),
	QObject(parent) {
	auto dbpath = QDir::homePath()+"/music.db";
	if(toDelete && !QFile::remove(dbpath)){
		throw Common::Exception("Can't remove " + dbpath.toStdString());
	}
	db.setDatabaseName(dbpath);
	if(!db.open()){
		throw Common::Exception("Can't open database");
	}
	QSqlQuery query;
	exec(query, DISABLE_SYNCHRONOUS);
	exec(query, MEMORY_JOURNAL);
	begin();
	exec(query, ENABLE_FOREIGN_KEYS);
	exec(query, CREATE_ARTIST_TABLE);
	exec(query, CREATE_GENRE_TABLE);
	exec(query, CREATE_MUSIC_TABLE);
	exec(query, CREATE_TAG_TABLE);
	exec(query, CREATE_MUSIC_TAG_TABLE);
	exec(query, CREATE_MUSIC_INDEX);
	exec(query, CREATE_TAG_INDEX);
	commit();
}

void MusicDb::fetchYoutube(){
	QThreadPool *threadPool = QThreadPool::globalInstance();
	threadPool->setMaxThreadCount(100);
	QSqlQuery music_query(db);
	exec(music_query, SELECT_MUSICS_WITHOUT_YOUTUBE.c_str());
	while(music_query.next()){
		auto id = music_query.value(0);
		QString title = music_query.value(1).toString();
		QString album = music_query.value(2).toString();
		QString artist = music_query.value(3).toString();
		auto work = new YoutubeFetcher(db, id, title, album, artist);
		work->setAutoDelete(true);
		threadPool->start(work);
	}
	threadPool->waitForDone();
}

void MusicDb::save(Collection& collection){
	begin();
	auto musics = collection.getMusics();
	for(auto music : musics){
		save(music.second);
	}
	commit();
}

void MusicDb::save(MusicFile * mf){
	QSqlQuery query(db);
	// insert the artist
	auto insert_artist = "INSERT OR IGNORE INTO artist (name) VALUES(:name)";
	query.prepare(insert_artist);
	query.bindValue(":name", QVariant(mf->getArtist().c_str()));
	exec(query);

	// insert the genre
	auto insert_genre = "INSERT OR IGNORE INTO genre (name) VALUES(:name)";
	query.prepare(insert_genre);
	query.bindValue(":name", QVariant(mf->getGenre().c_str()));
	exec(query);

	auto insert_music =
	R"(
		INSERT OR IGNORE INTO music (artist_id, genre_id, album, rating, duration, uuid, filepath, title)
		VALUES
		(
			(SELECT id FROM artist WHERE name = :artist),
			(SELECT id FROM genre WHERE name = :genre),
			:album,
			:rating,
			:duration,
			:uuid,
			:filepath,
			:title
		);
	)";
	query.prepare(insert_music);
	query.bindValue(":artist", QVariant(mf->getArtist().c_str()));
	query.bindValue(":genre", QVariant(mf->getGenre().c_str()));
	query.bindValue(":album", QVariant(mf->getAlbum().c_str()));
	query.bindValue(":rating", QVariant(mf->getRating()));
	query.bindValue(":duration", QVariant(mf->getDuration().c_str()));
	query.bindValue(":uuid", QVariant(mf->getUUID().c_str()));
	query.bindValue(":filepath", QVariant(mf->getFilepath().c_str()));
	query.bindValue(":title", QVariant(mf->getTitle().c_str()));
	//query.bindValue(":youtube", QVariant(mf->getYoutube().c_str()));
	exec(query);

	// upsert music
	auto update_music =
	R"(
		UPDATE OR IGNORE music
		SET
			genre_id=(SELECT id FROM genre WHERE name = :genre),
			artist_id=(SELECT id FROM artist WHERE name = :artist),
			album=:album,
			rating=:rating,
			duration=:duration,
			uuid=:uuid,
			filepath=:filepath,
			title=:title
		WHERE changes()=0 AND filepath = :filepath;
	)";
	query.prepare(update_music);
	query.bindValue(":artist", QVariant(mf->getArtist().c_str()));
	query.bindValue(":genre", QVariant(mf->getGenre().c_str()));
	query.bindValue(":album", QVariant(mf->getAlbum().c_str()));
	query.bindValue(":rating", QVariant(mf->getRating()));
	query.bindValue(":duration", QVariant(mf->getDuration().c_str()));
	query.bindValue(":uuid", QVariant(mf->getUUID().c_str()));
	query.bindValue(":filepath", QVariant(mf->getFilepath().c_str()));
	query.bindValue(":title", QVariant(mf->getTitle().c_str()));
	exec(query);

	// insert tags
	for(auto tag : mf->getSplittedKeywords())
	{
		auto insert_tag = "INSERT OR IGNORE INTO tag (name) VALUES(:name)";
		query.prepare(insert_tag);
		query.bindValue(":name", QVariant(tag.c_str()));
		exec(query);

		auto insert_music_tag = R"(
			INSERT OR IGNORE INTO music_tag (music_id, tag_id)
			VALUES ((SELECT id FROM music WHERE filepath = :filepath), (SELECT id FROM tag WHERE name = :tag))
		)";
		query.prepare(insert_music_tag);
		query.bindValue(":filepath", QVariant(mf->getFilepath().c_str()));
		query.bindValue(":tag", QVariant(tag.c_str()));
		exec(query);
	}
}

void MusicDb::generateBest(){
	QSqlQuery artist_query(db);
	exec(artist_query, SELECT_ARTISTS.c_str());
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
		exec(best);
		while (best.next()){
			for(int i = 0; i < best.record().count(); i++){
				qDebug() << best.value(i).toString();
			}
		}
	}
}

void MusicDb::generateBestByKeyword(){
	QSqlQuery artist_query(db);
	exec(artist_query, SELECT_ARTISTS.c_str());
	while(artist_query.next()){
		auto artist = artist_query.value(0);
		QSqlQuery tag_query(db);
		exec(tag_query, SELECT_TAGS.c_str());
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
			exec(best);
			while (best.next()){
				for(int i = 0; i < best.record().count(); i++){
					qDebug() << best.value(i).toString();
				}
			}
		}
	}
}

std::string SELECT_TAGS = "SELECT name FROM tag;";
std::string SELECT_ARTISTS = "SELECT name FROM artist;";
std::string SELECT_MUSICS_WITHOUT_YOUTUBE =
R"(
	SELECT
		m.id,
		m.title,
		m.album,
		a.name
	FROM music m
	INNER JOIN artist a ON m.artist_id = a.id
	WHERE m.youtube IS NULL or m.youtube = '';
)";

std::string DISABLE_SYNCHRONOUS = "PRAGMA synchronous = OFF;";
std::string MEMORY_JOURNAL = "PRAGMA journal_mode = MEMORY";
std::string ENABLE_FOREIGN_KEYS = "PRAGMA foreign_keys = ON;";
std::string CREATE_ARTIST_TABLE = "CREATE TABLE IF NOT EXISTS `artist`(`id` INTEGER PRIMARY KEY, `name` VARCHAR UNIQUE);";
std::string CREATE_GENRE_TABLE = "CREATE TABLE IF NOT EXISTS `genre` (`id` INTEGER PRIMARY KEY, `name` VARCHAR UNIQUE);";
std::string CREATE_MUSIC_TABLE =
R"(
	CREATE TABLE IF NOT EXISTS `music` (
		`id` INTEGER PRIMARY KEY,
		`artist_id` INTEGER,
		`genre_id` INTEGER,
		`rating` REAL,
		`duration` VARCHAR,
		`uuid` VARCHAR,
		`title` VARCHAR,
		`album` VARCHAR,
		`filepath` VARCHAR UNIQUE,
		`youtube` VARCHAR,
		FOREIGN KEY(`artist_id`) REFERENCES artist(id),
		FOREIGN KEY(`genre_id`) REFERENCES genre(id));
)";
std::string CREATE_TAG_TABLE = "CREATE TABLE IF NOT EXISTS `tag`(`id` INTEGER PRIMARY KEY, `name` VARCHAR UNIQUE);";
std::string CREATE_MUSIC_TAG_TABLE =
R"(
	CREATE TABLE IF NOT EXISTS `music_tag`(
		`music_id` INTEGER,
		`tag_id` INTEGER,
		PRIMARY KEY (`music_id`, `tag_id`));
)";
//FOREIGN KEY(`music_id`) REFERENCES music(id) ON DELETE CASCADE,
//FOREIGN KEY(`tag_id`) REFERENCES tag(id) ON DELETE CASCADE);
std::string CREATE_MUSIC_INDEX = "CREATE INDEX IF NOT EXISTS `musicindex` ON music_tag(music_id);";
std::string CREATE_TAG_INDEX = "CREATE INDEX IF NOT EXISTS `tagindex` ON music_tag(tag_id);";
