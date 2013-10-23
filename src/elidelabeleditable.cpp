#include "elidelabeleditable.hpp"

#include <QApplication>
#include <QEvent>

ElideLabelEditable::ElideLabelEditable(QWidget *parent) :
    ElideLabel(parent)
{
    this->installEventFilter(this);
}

bool ElideLabelEditable::eventFilter(QObject *obj, QEvent *ev)
{
    switch(ev->type()) {
    case QEvent::MouseButtonRelease:
        emit clicked();
        break;
    case QEvent::Enter:
        QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));
        break;
    case QEvent::Leave:
        QApplication::restoreOverrideCursor();
        break;
    default:
        return false;
    }

    return true;    //will have returned false already if event was unhandled by the switch statement
}
