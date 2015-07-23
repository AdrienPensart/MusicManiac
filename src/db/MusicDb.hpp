#pragma once

#include "lib/MusicFile.hpp"
#include "lib/Collection.hpp"
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

extern QString ENABLE_FOREIGN_KEYS;
extern QString CREATE_ARTIST_TABLE;
extern QString CREATE_GENRE_TABLE;
extern QString CREATE_MUSIC_TABLE;
extern QString CREATE_TAG_TABLE;
extern QString CREATE_MUSIC_INDEX;
extern QString CREATE_MUSIC_TAG_TABLE;
extern QString CREATE_TAG_INDEX;

class MusicDb : public QObject {
	Q_OBJECT
	public:

		MusicDb(QObject * parent = 0);
		void save(MusicFile * mf);
		void save(Collection& collection);

	private:
		QSqlDatabase db;
};
