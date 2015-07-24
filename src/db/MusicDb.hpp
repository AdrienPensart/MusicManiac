#pragma once

#include "lib/MusicFile.hpp"
#include "lib/Collection.hpp"
#include "common/Exception.hpp"
#include <QDebug>
#include <QRunnable>
#include <QObject>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>

extern std::string DISABLE_SYNCHRONOUS;
extern std::string SELECT_ARTISTS;
extern std::string ENABLE_FOREIGN_KEYS;
extern std::string CREATE_ARTIST_TABLE;
extern std::string CREATE_GENRE_TABLE;
extern std::string CREATE_MUSIC_TABLE;
extern std::string CREATE_TAG_TABLE;
extern std::string CREATE_MUSIC_INDEX;
extern std::string CREATE_MUSIC_TAG_TABLE;
extern std::string CREATE_TAG_INDEX;
extern std::string SELECT_TAGS;
extern std::string SELECT_MUSICS_WITHOUT_YOUTUBE;
extern std::string MEMORY_JOURNAL;

typedef Common::Exception MusicDbException;

class YoutubeFetcher : public QRunnable {
	public:
		YoutubeFetcher(QSqlDatabase& db, QVariant id, QString title, QString album, QString artist);
		void run();
		static std::string execCmd(std::string cmd);
	private:
		QSqlDatabase& db;
		QVariant id;
		QString title;
		QString album;
		QString artist;
};

class MusicDb : public QObject {
	Q_OBJECT
	public:

		MusicDb(bool toDelete=false, QObject * parent = 0);
		void save(MusicFile * mf);
		void save(Collection& collection);
		void generateBest();
		void generateBestByKeyword();
		void fetchYoutube();

	private:

		void begin();
		void commit();
		QSqlDatabase db;
};
