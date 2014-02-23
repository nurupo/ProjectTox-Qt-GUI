#include "smileytextobject.hpp"
#include <QPainter>


QSizeF SmileyTextObject::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    // TODO MKO Quelle des Bildes
    QImage bufferedImage = QImage(":/icons/emoticons/emotion_angel.png");

    QSize size = bufferedImage.size();

    if (size.height() > 25)
        size *= 25.0 / (double) size.height();

    return QSizeF(size);
}

void SmileyTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    // TODO MKO Quelle des Bildes
    QImage bufferedImage = QImage(":/icons/emoticons/emotion_angel.png");

    painter->drawImage(rect, bufferedImage);
}
