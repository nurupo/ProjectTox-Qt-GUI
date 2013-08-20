#ifndef EMOTICONMENU_H
#define EMOTICONMENU_H

#include <QMenu>

class QGridLayout;

/*! This Class represents a menu with all smileys for adding into text input field.
 * And it holds a hash of the smileys for translating image to text.
 */
class EmoticonMenu : public QMenu
{
    Q_OBJECT
public:
    typedef QHash<QString,QStringList> SmileyHash; //!< Hash of all smiley image pathes (key) and lists of their textual trpresantations (values).

    explicit EmoticonMenu(QWidget *parent = 0);

    SmileyHash getSmileyList();
    
signals:
    void insertEmoticon(QString);
    
public slots:
    
private slots:
    void onEmoticonTriggered();

private:
    void addEmoticon(const QString &imgPath, const QStringList &texts);

    QGridLayout *layout;
    SmileyHash smileyList;
};

#endif // EMOTICONMENU_H
