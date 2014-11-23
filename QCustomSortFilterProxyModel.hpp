#ifndef QCUSTOMSORTFILTERPROXYMODEL_HPP
#define QCUSTOMSORTFILTERPROXYMODEL_HPP

#include <QSortFilterProxyModel>
#include <QStringListModel>

class QCustomSortFilterProxyModel : public QSortFilterProxyModel
{
    public:

        QCustomSortFilterProxyModel(QStringListModel& without, QStringListModel& with, QObject * parent=0);
        void refilter();

    protected:

        virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;
        QStringListModel& without;
        QStringListModel& with;
};

#endif // QCUSTOMSORTFILTERPROXYMODEL_HPP
