#pragma once

#include "lib/MusicFile.hpp"
#include "lib/Collection.hpp"
#include "common/Exception.hpp"
#include <QObject>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>

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

typedef Common::Exception MusicDbException;

class MusicDb : public QObject {
	Q_OBJECT
	public:

		MusicDb(QObject * parent = 0);
		void save(MusicFile * mf);
		void save(Collection& collection);
		void generateBest();
		void generateBestByKeyword();

	private:

		void exec(QSqlQuery& query, const std::string& sql="");
		QSqlDatabase db;
};
