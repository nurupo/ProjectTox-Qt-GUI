#ifndef MESSAGEMODEL_HPP
#define MESSAGEMODEL_HPP

#include <QAbstractItemModel>

class MessageModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit MessageModel(QObject *parent = 0);

signals:

public slots:

};

#endif // MESSAGEMODEL_HPP
