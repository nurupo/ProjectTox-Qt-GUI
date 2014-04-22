#ifndef CHATVIEWSEARCHWIDGET_HPP
#define CHATVIEWSEARCHWIDGET_HPP

#include <QToolBar>
#include "message.hpp"

class ChatScene;
class QTextCursor;
class ChatItem;
class ChatLine;
class QLineEdit;
class Highlight;
class SmileyList;

class ChatViewSearchWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit ChatViewSearchWidget(QWidget *parent = 0);
    void setScene(ChatScene *scene);

public slots:
    void setSearchString(const QString &searchString);
    void setCaseSensitive(bool caseSensitive);
    void setSearchOnlyRegularMsgs(bool searchOnlyRegularMsgs);
    void highlightNext();
    void highlightPrev();
    void rowsRemoved(int start, int end);

signals:
    void newCurrentHighlight(QPointF position);

private slots:
    void sceneDestroyed();
    void updateHighlights(bool reuse = false);
    inline void updateExistingHighlights() { updateHighlights(true); }
    void closeSearch();

private:
    struct Offsets {
        int length;
        int start;
    };

    void checkMessagesForHighlight(int start = 0, int end = -1);
    void findHighlightInItem(ChatItem *item);
    void updateHighlights(ChatItem *item);
    void updateCurrentHighlight(ChatItem *oldItem, int oldStart);
    inline bool checkType(Message::Type type) const { return type & (Message::Plain | Message::Action); }
    QList<int> findWords(const QString &string);
    void clearHightlights();
    Offsets calculateSmileyOffsets(const SmileyList *smileys, int foundWordStart);

    ChatScene *mScene;
    QString mSearchString;
    QLineEdit *mSearchLineEdit;
    Qt::CaseSensitivity mCaseSensitive;
    bool mRegularMsgOnly;

    int mCurrentHighlight;
    QList<Highlight*> mHighlights; // A list of pointers to all highlights in all ContentItems
};

#endif // CHATVIEWSEARCHWIDGET_HPP
