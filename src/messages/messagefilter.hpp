#ifndef MESSAGEFILTER_HPP
#define MESSAGEFILTER_HPP

#include <QSortFilterProxyModel>
#include "message.hpp"

class MessageFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MessageFilter(QObject *parent = 0);

signals:

public slots:
    inline void filterPlain(bool hide)     { filterMessageType(hide, Message::Plain); }
    inline void filterAction(bool hide)    { filterMessageType(hide, Message::Action); }
    inline void filterNick(bool hide)      { filterMessageType(hide, Message::Nick); }
    inline void filterError(bool hide)     { filterMessageType(hide, Message::Error); }
    inline void filterDaychange(bool hide) { filterMessageType(hide, Message::DayChange); }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    int mTypesToHide;

    void filterMessageType(bool hide, Message::Type type);
};

#endif // MESSAGEFILTER_HPP
