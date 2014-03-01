/*
    Copyright (C) 2013 by Wei-Ning Huang <aitjcize@gmail.com>
    
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

#ifndef FILETRANSFERSTATE_HPP
#define FILETRANSFERSTATE_HPP

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QFile>

class FileTransferState: public QObject
{
    Q_OBJECT

public:
    enum {
      RECEIVE = 0,
      SEND
    };

    static bool checkPermission(const QString& filename, int mode = RECEIVE);

    FileTransferState(int friendId, int filenumber, quint64 filesize,
        const QString& filename, int mode = RECEIVE);
    ~FileTransferState();

    QString fileName();
    int friendId();
    int fileNumber();

    int writeData(const QByteArray& data);
    qint64 readData(char* buf, int max_size);
    void readComplete();

    void createBuf(int size);
    char* buffer();
    int chunkSize();

signals:
    void progressChanged(int percentage);

private:
    int friendid;
    int filenumber;
    QString filename;
    quint64 filesize;
    quint64 transfered;
    int readLength;
    bool ack;
    QFile file;

    char* buf;
    int chunksize;
};

#endif /* FILETRANSFERSTATE_HPP */
