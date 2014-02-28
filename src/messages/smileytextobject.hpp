#ifndef SMILEYTEXTOBJECT_HPP
#define SMILEYTEXTOBJECT_HPP

#include <QTextObjectInterface>

class SmileyTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
public:
    SmileyTextObject(const QString &pixmap);

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

private:
    QImage mImage;
};

#endif // SMILEYTEXTOBJECT_HPP
