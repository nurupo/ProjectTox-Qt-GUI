#include "messagestyleinterface.h"

#include <QRegularExpression>
#include <QTime>
#include <QDebug>
#include <QTextDocument>
#include <QTextTable>
#include <QTextDocumentFragment>
#include <QScrollBar>

#include "Settings/settings.hpp"

void MessageStyleInterface::urlify(QString &string)
{
    string.replace(QRegularExpression("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>");
}

// ///////////////////////////////////////////////////////////////////////////////////////////// //
// MessageDefaultStyle                                                                           //
// ///////////////////////////////////////////////////////////////////////////////////////////// //

void MessageDefaultStyle::initStyle()
{
    mWidget->document()->setDefaultStyleSheet("p {text-indent:-10px; margin-left:10px; margin-top:5; margin-bottom:0; white-space:pre-wrap;}");
}

void MessageDefaultStyle::addMessage(const QString &name, const QString &message)
{
    QString nameEscaped = name.toHtmlEscaped();
    QString timeEscaped = QTime::currentTime().toString("hh:mm:ss").toHtmlEscaped();
    QString messageEscaped = message.toHtmlEscaped();

    QString text = QString("<p><b>%1</b> [%2]<br>%3</p>").arg(nameEscaped).arg(timeEscaped).arg(messageEscaped);
    urlify(text);
    mWidget->append(text);
}

void MessageDefaultStyle::addErrorMessage(const QString &message)
{
    QString messageEscaped = message.toHtmlEscaped();
    QString text = QString("<p><font color=\"red\" style=\"font-weight:bold\">Couldn't send following message:</font><br>%1</p>").arg(messageEscaped);
    urlify(text);
    mWidget->append(text);
}

// ///////////////////////////////////////////////////////////////////////////////////////////// //
// MessageIrcStyle                                                                               //
// ///////////////////////////////////////////////////////////////////////////////////////////// //

void MessageIrcStyle::initStyle()
{
    table = NULL;
}

void MessageIrcStyle::addMessage(const QString &xName, const QString &message)
{
    // Prepare message strings
    QString name = QString("<%1>").arg(xName);
    QString time = QTime::currentTime().toString("[hh:mm:ss]");
    QString text = message;
    urlify(text);

    // Create table is there is no
    QTextCursor cursor(mWidget->document());
    if(!table)
    {
        table = cursor.insertTable(1,3);

        // Create Format
        QTextTableFormat tableFormat;
        tableFormat.setBorderStyle(QTextTableFormat::BorderStyle_None);
        tableFormat.setCellSpacing(0);
        tableFormat.setCellPadding(0);
        table->setFormat(tableFormat);
    }
    else
        table->appendRows(1);

    // Find cells of last row
    cursor = table->lastCursorPosition();
    QTextTableCell cellTime = table->cellAt(table->cellAt(cursor).row(),0);
    QTextTableCell cellName = table->cellAt(table->cellAt(cursor).row(),1);
    QTextTableCell cellMsg  = table->cellAt(table->cellAt(cursor).row(),2);

    // Color name
    const Settings& settings = Settings::getInstance();
    QColor col;
    if(xName == settings.getUsername())
        col = Qt::darkGreen;
    else
        col = Qt::darkRed;

    // Find last used name and insert name
    QString lastName;
    if(table->rows() > 1){
        while(table->cellAt(cursor).row() > 0)
        {
            QTextTableCell cellLastName = table->cellAt(table->cellAt(cursor).row()-1,1);
            cursor = cellLastName.firstCursorPosition();
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            QString testName = cursor.selection().toPlainText();
            if(!testName.isEmpty())
            {
                lastName = testName;
                break;
            }
        }

        // insert name if not equals last name
        if( name != lastName && !lastName.isEmpty() )
            insertText(&cellName,  name, Qt::AlignRight, col);
    }
    else
        insertText(&cellName,  name, Qt::AlignRight, col);

    // Insert time and message
    insertText(&cellTime, time);
    insertText(&cellMsg,  text);

    // Scroll to bottom
    QScrollBar *scrollbar = mWidget->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void MessageIrcStyle::addErrorMessage(const QString &message)
{
    // Create table is there is no
    QTextCursor cursor(mWidget->document());
    if(!table)
    {
        table = cursor.insertTable(1,3);

        // Create Format
        QTextTableFormat tableFormat;
        tableFormat.setBorderStyle(QTextTableFormat::BorderStyle_None);
        tableFormat.setCellSpacing(0);
        tableFormat.setCellPadding(0);
        table->setFormat(tableFormat);
    }
    else
        table->appendRows(1);

    // Find cells of last row
    cursor = table->lastCursorPosition();
    QTextTableCell cellTime = table->cellAt(table->cellAt(cursor).row(),0);
    QTextTableCell cellName = table->cellAt(table->cellAt(cursor).row(),1);
    QTextTableCell cellMsg  = table->cellAt(table->cellAt(cursor).row(),2);

    // Insert text
    QString time = QTime::currentTime().toString("[hh:mm:ss]");
    insertText(&cellTime, time,    Qt::AlignLeft,  Qt::red);
    insertText(&cellName, "*",     Qt::AlignRight, Qt::red);
    insertText(&cellMsg,  message, Qt::AlignLeft,  Qt::red);

    // Scroll to bottom
    QScrollBar *scrollbar = mWidget->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

/*! Insert text in specified cell */
void MessageIrcStyle::insertText(QTextTableCell *cell, const QString &text, Qt::Alignment alignment, QColor color)
{
    // Obtain cursor and current block format
    QTextCursor textCursor = cell->firstCursorPosition();
    QTextBlockFormat blockFormat = textCursor.blockFormat();

    // set color
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(color));
    cell->setFormat(fmt);

    // Read vertical part of current alignment flags
    Qt::Alignment vertAlign = blockFormat.alignment() & Qt::AlignVertical_Mask;

    // Mask out vertical part of specified alignment flags
    Qt::Alignment horzAlign = alignment & Qt::AlignHorizontal_Mask;

    // Combine current vertical and specified horizontal alignment
    Qt::Alignment combAlign = horzAlign | vertAlign;

    // Apply and write
    blockFormat.setAlignment(combAlign);
    textCursor.setBlockFormat(blockFormat);
    textCursor.insertText(text);
}
