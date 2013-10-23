#ifndef ELIDELABELEDITABLE_HPP
#define ELIDELABELEDITABLE_HPP

#include "elidelabel.hpp"

class ElideLabelEditable : public ElideLabel
{
    Q_OBJECT
public:
    explicit ElideLabelEditable(QWidget *parent = 0);
    
signals:
    void clicked();

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    
};

#endif // ELIDELABELEDITABLE_HPP
