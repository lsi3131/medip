#pragma once

#ifndef FOLDER_COMPRESSOR_H
#define FOLDER_COMPRESSOR_H

#include <QFile>
#include <QObject>
#include <QDir>
#include <qdatastream.h>

class FolderCompressor : public QObject
{
    Q_OBJECT
public:
    explicit FolderCompressor(QObject *parent = 0);

    bool compressFolder(QString sourceFolder, QString destinationFile);
    bool decompressFolder(QString sourceFile, QString destinationFolder);
	bool loadResourceFromCompressFile(QString sourceFile);
private:
    QFile file;
    QDataStream dataStream;

    bool compress(QString sourceFolder, QString prefex);
};
#endif
