#include "smileytextobject.hpp"
#include <QPainter>


SmileyTextObject::SmileyTextObject(const QString &pixmap)
{
    mImage = QImage(pixmap);
}

QSizeF SmileyTextObject::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    QSize size = mImage.size();

    if (size.height() > 25)
        size *= 25.0 / (double) size.height();

    return QSizeF(size);
}

void SmileyTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    painter->drawImage(rect, mImage);
}
