#pragma once

#include <QDateTime>
#include <QSqlQuery>
#include <QFile>
#include <QUrl>
#include "notesubfolder.h"
#include "note.h"


class TrashItem {
public:
    explicit TrashItem();

    int getId() const;

    QString getFileName() const;

    static TrashItem fetch(int id);

    static QList<TrashItem> fetchAll(int limit = -1);

    bool store();

    friend QDebug operator<<(QDebug dbg, const TrashItem &trashItem);

    bool fileExists() const;

    bool exists() const;

    bool refetch();

    bool fillFromQuery(const QSqlQuery& query);

    bool removeFile();

    bool remove(bool withFile = false);

    bool isFetched() const;

    QDateTime getCreated() const;

    static int countAll();

    QString fileBaseName(bool withFullName = false) const;

    NoteSubFolder getNoteSubFolder() const;

    void setNoteSubFolder(const NoteSubFolder &noteSubFolder);

    QString relativeNoteFilePath(const QString &separator = QString());

    QString getNoteSubFolderPathData() const;

    qint64 getFileSize() const;

    static TrashItem trashItemFromQuery(const QSqlQuery& query);

    static bool deleteAll();

    bool fillFromId(int id);

    static bool add(const Note &note);

    static bool add(const Note *note);

    void setNote(const Note &note);

    void setNote(const Note *note);

    static TrashItem prepare(const Note *note);

    bool doTrashing();

    QString fullFilePath() const;

    QString loadFileFromDisk();

    bool restoreFile();

    QString restorationFilePath() const;

    static bool isLocalTrashEnabled();

    static bool expireItems();

    static QList<TrashItem> fetchAllExpired();

protected:
    int id;
    QString fileName;
    qint64 fileSize;
    QString noteSubFolderPathData;
    int noteSubFolderId;
    QDateTime created;
    QString _fullNoteFilePath;
};
