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

#include "filetransferstate.hpp"
#include <QDebug>

FileTransferState::FileTransferState(int _friendId, int _filenumber, quint64 _filesize, const QString& _filename, int mode)
    : file(_filename)
{
    friendid = _friendId;
    filenumber = _filenumber;
    filename = _filename;
    filesize = _filesize;;
    transfered = 0;
    buf = NULL;
    ack = true;

    QIODevice::OpenMode m = (mode == RECEIVE)? QIODevice::WriteOnly: QIODevice::ReadOnly;
    file.open(m);
}

FileTransferState::~FileTransferState()
{
    file.close();
    if (buf) {
        delete [] buf;
    }
}


bool FileTransferState::checkPermission(const QString& filename, int mode)
{
    QIODevice::OpenMode m = (mode == RECEIVE)? QIODevice::WriteOnly: QIODevice::ReadOnly;
  return QFile(filename).open(m);
}

QString FileTransferState::fileName()
{
    return filename;
}

int FileTransferState::friendId()
{
    return friendid;
}

int FileTransferState::fileNumber()
{
    return filenumber;
}

int FileTransferState::writeData(const QByteArray& data)
{
    qint64 len = file.write(data.data(), data.length());
    if (len != data.length()) {
        return -1;
    }
    transfered += len;
    emit progressChanged((int)(transfered * 100.0 / filesize));
    return len;
}

qint64 FileTransferState::readData(char* buf, int max_size)
{
    if (ack) {
        readLength = file.read(buf, max_size);
        ack = false;
    }
    return readLength;
}

void FileTransferState::readComplete() {
    ack = true;
    transfered += readLength;
    emit progressChanged((int)(transfered * 100.0 / filesize));
}

void FileTransferState::createBuf(int size)
{
    if (buf) {
        delete [] buf;
    }
    chunksize = size;
    buf = new char[size];
}

char* FileTransferState::buffer()
{
    return buf;
}

int FileTransferState::chunkSize()
{
    return chunksize;
}
