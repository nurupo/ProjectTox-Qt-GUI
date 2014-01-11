/*
    Copyright (C) 2013 by Maxim Biro <nurupo.contributions@gmail.com>
    
    This file is part of Tox Qt GUI.
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    
    See the COPYING file for more details.
*/

#include "chatpagewidget.hpp"
#include "ouruseritemwidget.hpp"
#include "status.hpp"
#include "Settings/settings.hpp"
#include "customhintwidget.hpp"
#include "messagedisplaywidget.hpp"
#include "emoticonmenu.hpp"

#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

ChatPageWidget::ChatPageWidget(int friendId, QWidget* parent) :
    QWidget(parent), friendId(friendId)
{
    friendItem = new FriendItemWidget(this);
    display = new MessageDisplayWidget(this);

    input = new InputTextWidget(this);
    connect(input, &InputTextWidget::sendMessage, this, &ChatPageWidget::sendMessage);
    connect(input, &InputTextWidget::sendAction,  this, &ChatPageWidget::sendAction);

    // Create emoticon menu :)
    CustomHintWidget *inputPanel = new CustomHintWidget(this, QSize(10, 10));
    EmoticonMenu *menu = new EmoticonMenu(this);
    emoticonButton = new QToolButton(inputPanel);
    emoticonButton->setPopupMode(QToolButton::InstantPopup);
    emoticonButton->setIcon(QIcon(":/icons/emoticons/emotion_smile.png"));
    emoticonButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    emoticonButton->setMenu(menu);
    connect(menu, &EmoticonMenu::insertEmoticon, input, &InputTextWidget::insertHtml);

    filesendButton = new QToolButton(this);
    filesendButton->setIcon(QIcon(":/icons/attach.png"));
    filesendButton->setToolTip(tr("Send file"));
    filesendButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(filesendButton, &QToolButton::clicked, this, &ChatPageWidget::promptSendFile);

    QVBoxLayout *buttonlayout = new QVBoxLayout();
    buttonlayout->addWidget(emoticonButton);
    buttonlayout->addWidget(filesendButton);

    QHBoxLayout *inputLayout = new QHBoxLayout(inputPanel);
    inputLayout->setContentsMargins(0,0,0,0);
    inputLayout->setSpacing(2);
    inputLayout->addWidget(input);
    inputLayout->addLayout(buttonlayout);

    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(display);
    splitter->addWidget(inputPanel);
    splitter->setStretchFactor(0, 3);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(friendItem);
    layout->addWidget(splitter);
    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 2, 3);
}

int ChatPageWidget::getFriendId() const
{
    return friendId;
}

void ChatPageWidget::messageReceived(const QString& message)
{
    display->appendMessage(username, message, -1, false);
}

void ChatPageWidget::setUsername(const QString& newUsername)
{
    username = newUsername;
    friendItem->setUsername(username);
}

void ChatPageWidget::setStatus(Status newStatus)
{
    status = newStatus;
    friendItem->setStatus(status);
    input->setDisabled(newStatus == Status::Offline);
    emoticonButton->setDisabled(newStatus == Status::Offline);
}

void ChatPageWidget::setStatusMessage(const QString& statusMessage)
{
    friendItem->setStatusMessage(statusMessage);
}

void ChatPageWidget::messageSentResult(const QString& message, int messageId)
{
    display->appendMessage(Settings::getInstance().getUsername(), message, messageId, true);
}

void ChatPageWidget::actionReceived(const QString &message)
{
    display->appendAction(username, message, false);
}

void ChatPageWidget::actionSentResult(const QString &message)
{
    display->appendAction(username, message, true);
}

quint8 ChatPageWidget::fileSendReceived(quint8 filenumber, quint64 filesize, const QString& filename)
{
  QMessageBox::StandardButton ret = QMessageBox::question(this,
      tr("File Transfer"),
      tr("%1 is sending you a file `%2', accept?").arg(username).arg(filename));

  quint8 msg_id = TOX_FILECONTROL_KILL;

  if (ret == QMessageBox::Yes) {
    QString saveFilename = QFileDialog::getSaveFileName(this,
        tr("Save file"), filename);

    if (!FileTransferState::checkPermission(saveFilename)) {
      QMessageBox::critical(this, tr("Error"),
          tr("Failed to open `%1' for writing").arg(saveFilename));
    } else {
      FileTransferState* state = new FileTransferState(friendId, filenumber,
          filesize, saveFilename);
      states.insert(filenumber, state);
      display->appendProgress(saveFilename, state, false);
      msg_id = TOX_FILECONTROL_ACCEPT;
    }
  }
  return msg_id;
}

void ChatPageWidget::fileControlReceived(unsigned int receive_send, quint8 filenumber, quint8 control_type, const QByteArray& data)
{
    if (receive_send == 0 && control_type == TOX_FILECONTROL_FINISHED) {
        FileTransferState* state = states[filenumber];
        states.remove(filenumber);
        delete state;
    } else if (receive_send == 1 && control_type == TOX_FILECONTROL_ACCEPT) {
        FileTransferState* state = new FileTransferState(friendId, filenumber,
                QFileInfo(currentSendFilename).size(), currentSendFilename,
                FileTransferState::SEND);
        states.insert(filenumber, state);
        display->appendProgress(currentSendFilename, state, true);
        emit sendFile(state);
    }
}

void ChatPageWidget::fileDataReceived(quint8 filenumber, const QByteArray& data)
{
  FileTransferState* state = states[filenumber];
  if (state->writeData(data) == -1) {
    QMessageBox::critical(this, tr("Error"),
        tr("failed to write data for `%1'").arg(state->fileName()));
    states.remove(filenumber);
    delete state;
  }
}

void ChatPageWidget::fileSendCompletedReceived(int filenumber)
{
  FileTransferState* state = states[filenumber];
  states.remove(filenumber);
  delete state;
}

void ChatPageWidget::promptSendFile(void)
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Select a file"));
  if (filename.length()) {
      if (!FileTransferState::checkPermission(filename,
                  FileTransferState::SEND)) {
          QMessageBox::critical(this, tr("Error"),
                  tr("Failed to open `%1' for reading").arg(filename));
      } else {
          currentSendFilename = filename;
          emit sendFileRequest(filename);
      }
  }
}
