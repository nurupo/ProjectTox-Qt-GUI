#ifndef CHATVIEWSEARCHWIDGET_HPP
#define CHATVIEWSEARCHWIDGET_HPP

#include <QWidget>
#include "message.hpp"

class ChatScene;
class QTextCursor;
class ChatItem;
class ChatLine;
class QLineEdit;
class Hightlight;
class SmileyList;

class ChatViewSearchWidget : public QWidget
{
    Q_OBJECT
public:
    class SearchItemCursor {
    public:
        SearchItemCursor() : start(-1), end(-1), item(nullptr) {}
        bool operator==(SearchItemCursor const &other) const;
        int       start;
        int       end;
        ChatItem *item;
    };

    explicit ChatViewSearchWidget(QWidget *parent = 0);
    void setScene(ChatScene *scene);

    inline const QString &searchString() const { return _searchString; }

public slots:
    void setSearchString(const QString &searchString);
    void setCaseSensitive(bool caseSensitive);
    void setSearchOnlyRegularMsgs(bool searchOnlyRegularMsgs);

    void highlightNext();
    void highlightPrev();


private slots:
    void sceneDestroyed();
    void updateHighlights(bool reuse = false);
    void updateExistingHighlights() { updateHighlights(true); }
    void closeSearch();

private:
    struct Offsets{
        int length;
        int start;
    };


    void checkMessagesForHighlight(int start = 0, int end = -1);
    void highlightLine(ChatItem *line);
    void updateHighlights(ChatItem *item);
    inline bool checkType(Message::Type type) const { return type & (Message::Plain | Message::Action); }
    QList<int> findWords(const QString &string);
    void clearHightlights();
    Offsets calculateSmileyOffsets(const SmileyList *smileys, int foundWordStart);

    QString _searchString;
    Qt::CaseSensitivity _caseSensitive;
    bool _searchOnlyRegularMsgs;

    ChatScene *_scene;
    QLineEdit *searchLineEdit;

    QList<SearchItemCursor> _highlightCursors;
    int _currentHighlight;
    QList<Hightlight*> mHighlights;
};
uint qHash(const ChatViewSearchWidget::SearchItemCursor &key, uint seed);

#endif // CHATVIEWSEARCHWIDGET_HPP
