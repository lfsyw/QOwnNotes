#pragma once

#include <QDateTime>
#include <QSqlQuery>
#include <QDebug>
#include <QDir>

// note sub-folders that should be ignored by default
// regular expression, separated by ";"
#define IGNORED_NOTE_SUBFOLDERS_DEFAULT "^\\."

class NoteSubFolder {
public:
    explicit NoteSubFolder();

    int getId() const;

    QString getName() const;

    void setName(const QString &text);

    static NoteSubFolder fetch(int id);

    static QList<NoteSubFolder> fetchAll(int limit = -1);

    static NoteSubFolder noteSubFolderFromQuery(QSqlQuery query);

    bool store();

    friend QDebug operator<<(QDebug dbg, const NoteSubFolder &note);

    static bool deleteAll();

    bool exists();

    bool fillFromQuery(QSqlQuery query);

    bool remove();

    bool isFetched();

    QDateTime getFileLastModified();

    QDateTime getModified();

    static int countAll();

    int getParentId();

    void setParentId(int parentId);

    QString relativePath(const QString &separator = "") const;

    QString fullPath() const;

    NoteSubFolder getParent() const;

    static QList<NoteSubFolder> fetchAllByParentId(int parentId,
            const QString &sortBy = "file_last_modified DESC");

    static QList<int> fetchIdsRecursivelyByParentId(int parentId);

    bool isActive() const;

    static int activeNoteSubFolderId();

    static NoteSubFolder activeNoteSubFolder();

    static bool setAsActive(int noteSubFolderId);

    void setAsActive();

    QString pathData();

    static NoteSubFolder fetchByPathData(const QString &pathData,
                                         const QString &separator = "\n");

    static NoteSubFolder fetchByNameAndParentId(const QString &name, int parentId);

    void saveTreeWidgetExpandState(bool expanded);

    bool treeWidgetExpandState();

    static QString treeWidgetExpandStateSettingsKey(int noteFolderId = 0);

    bool removeFromFileSystem();

    QDir dir() const;

    bool rename(const QString &newName);

    static int countAllParentId(int parentId);

    static QList<int> fetchAllIds();

    static bool isNoteSubfoldersPanelShowNotesRecursively();

protected:
    int id;
    int parentId;
    QString name;
    QDateTime fileLastModified;
    QDateTime created;
    QDateTime modified;

signals:

public slots:
};
