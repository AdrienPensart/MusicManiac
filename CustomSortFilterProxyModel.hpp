#pragma once

#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QDoubleSpinBox>

class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:

        explicit CustomSortFilterProxyModel(QStringListModel& without, QStringListModel& with, QObject * parent=0);
        void refilter();

    public slots:

        void minDurationChanged(QString);
        void maxDurationChanged(QString);
        void ratingChanged(double);

    protected:

        virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

    private:

        double rating;
        QString minDuration;
        QString maxDuration;
        QStringListModel& without;
        QStringListModel& with;
};
