#include "messagefilter.hpp"
#include "messagemodel.hpp"
#include <QDebug>

MessageFilter::MessageFilter(QObject *parent) :
    QSortFilterProxyModel(parent),
    mTypesToHide(0)
{
}

bool MessageFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    QModelIndex index = sourceModel()->index(sourceRow, 1);
    Message::Type messageType = (Message::Type) index.data(MessageModel::TypeRole).toInt();

    return (messageType & mTypesToHide) ? false : true;
}

void MessageFilter::filterMessageType(bool hide, Message::Type type)
{
    int old = mTypesToHide;
    mTypesToHide = (hide) ? old | type : old & ~type;

    if(old != mTypesToHide)
        invalidateFilter();
}
