#include "CustomSortFilterProxyModel.hpp"
#include "MusicFolderModel.hpp"
#include <common/Utility.hpp>
#include <QSet>
#include <QDebug>

CustomSortFilterProxyModel::CustomSortFilterProxyModel(QStringListModel& _artists, QStringListModel& _without, QStringListModel& _with, QObject * parent)
	:
	QSortFilterProxyModel(parent),
	rating(0),
	minDuration("00:00"),
	maxDuration("100:00"),
	artists(_artists),
	without(_without),
	with(_with) {
}

void CustomSortFilterProxyModel::refilter() {
	setFilterFixedString("no-op");
}

void CustomSortFilterProxyModel::ratingChanged(double _rating) {
	rating = _rating;
	refilter();
}

void CustomSortFilterProxyModel::minDurationChanged(QString _minDuration) {
	minDuration = _minDuration;
	refilter();
}

void CustomSortFilterProxyModel::maxDurationChanged(QString _maxDuration) {
	maxDuration = _maxDuration;
	refilter();
}

QStringList CustomSortFilterProxyModel::getKeywords(){
	QStringList qsl;
	for(int row = 0; row < rowCount(); row++){
		QModelIndex keywords_index = index(row, MusicFolderModel::COLUMN_KEYWORDS);
		QString keywords = data(keywords_index).toString();
		std::vector<std::string> currentKeywords;
		Common::split(keywords.toStdString(), " ", currentKeywords);
		for(std::vector<std::string>::const_iterator si = currentKeywords.begin(); si != currentKeywords.end(); si++) {
			qsl.append(QString::fromStdString(*si));
		}
	}

	QStringList sorted = qsl.toSet().toList();
	sorted.sort();
	return sorted;
}

bool CustomSortFilterProxyModel::filterAcceptsRow (int sourceRow, const QModelIndex& sourceParent) const {
	QAbstractItemModel * model = sourceModel();

	QModelIndex artist_index = model->index(sourceRow, MusicFolderModel::COLUMN_ARTIST, sourceParent);
	QString currentArtist = model->data(artist_index).toString();
	if(!artists.stringList().contains(currentArtist)) {
		return false;
	}

	QModelIndex keywords_index = model->index(sourceRow, MusicFolderModel::COLUMN_KEYWORDS, sourceParent);
	QString keywords = model->data(keywords_index).toString();
	foreach(QString keyword, without.stringList()) {
		if(keywords.contains(keyword)) {
			return false;
		}
	}

	if(with.stringList().size()) {
		bool found = false;
		foreach(QString keyword, with.stringList()) {
			QRegExp reg("\\b"+keyword+"\\b");
			if(reg.indexIn(keywords) != -1) {
				found = true;
				break;
			}
		}
		if(!found) {
			return false;
		}
	}

	QModelIndex rating_index = model->index(sourceRow, MusicFolderModel::COLUMN_RATING, sourceParent);
	double currentRating = model->data(rating_index).toDouble();
	if(currentRating < rating) {
		return false;
	}

	QModelIndex duration_index = model->index(sourceRow, MusicFolderModel::COLUMN_DURATION, sourceParent);
	QString currentDuration = model->data(duration_index).toString();

	QString tempCurrentDuration = currentDuration;
	tempCurrentDuration.remove(':');
	unsigned int current = tempCurrentDuration.toUInt();

	QString tempMinDuration = minDuration;
	tempMinDuration.remove(':');
	unsigned int min = tempMinDuration.toUInt();

	QString tempMaxDuration = maxDuration;
	tempMaxDuration.remove(':');
	unsigned int max = tempMaxDuration.toUInt();

	if(min > current || max < current) {
		return false;
	}
	return true;
}
