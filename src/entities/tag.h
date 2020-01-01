#pragma once

#include <QSqlQuery>
#include <QList>
#include <QColor>
#include "note.h"

class Tag {
public:
    enum SpecialTag {
        AllNotesId = -1,
        AllUntaggedNotesId = -2,
    };

    explicit Tag();

    bool operator ==(const Tag &tag) const;

    bool operator <(const Tag &tag) const;

    friend QDebug operator<<(QDebug dbg, const Tag &tag);

    int getId() const;

    bool store();

    bool exists() const;

    bool fillFromQuery(const QSqlQuery& query);

    bool remove() const;

    bool isFetched() const;

    QString getName() const;

    void setName(const QString &text);

    int getPriority() const;

    void setPriority(const int value);

    void setAsActive();

    bool isActive() const;

    bool linkToNote(const Note &note) const;

    bool removeLinkToNote(const Note &note) const;

    QStringList fetchAllLinkedNoteFileNames(const bool fromAllSubfolders) const;

    QStringList fetchAllLinkedNoteFileNamesForFolder(const NoteSubFolder &noteSubFolder,
                                                     bool fromAllSubfolders) const;

    QList<Note> fetchAllLinkedNotes() const;

    bool isLinkedToNote(const Note &note) const;

    int countLinkedNoteFileNames(const bool fromAllSubfolder, const bool recursive) const;

    int countLinkedNoteFileNamesForNoteFolder(const NoteSubFolder &noteSubFolder, const bool recursive) const;

    int getParentId() const;

    void setParentId(const int id);

    bool hasChild(const int tagId) const;

    QColor getColor() const;

    void setColor(const QColor &color);

/*
 * Tag
 * Public static functions
*/

    static QList<Tag> fetchAll();

    static int countAll();

    static int activeTagId();

    static Tag activeTag();

    static Tag fetchByName(const QString &name, const bool startsWith = false);

    static Tag fetchByName(const QString &name, const int parentId);

    static QList<Tag> fetchAllOfNote(const Note &note);

    static QStringList fetchAllNames();

    static bool removeAllLinksToNote(const Note &note);

    static bool renameNoteFileNamesOfLinks(const QString& oldFileName,
                                           const QString& newFileName);

    static bool renameNoteSubFolderPathsOfLinks(const QString &oldPath,
                                                const QString &newPath);

    static Tag fetch(const int id);

    static Tag tagFromQuery(const QSqlQuery& query);

    static QList<Tag> fetchAllWithLinkToNoteNames(const QStringList& noteNameList);

    static QList<Tag> fetchAllByParentId(const int parentId, const QString& sortBy = "created DESC");

    static int countAllParentId(const int parentId);

    static int countAllOfNote(const Note &note);

    static void setAsActive(const int tagId);

    static void convertDirSeparator();

    static Tag fetchOneOfNoteWithColor(const Note& note);

    static void migrateDarkColors();

    static void removeBrokenLinks();

    static QStringList fetchAllNamesOfNote(const Note &note);

    static QStringList searchAllNamesByName(const QString& name);

    static QList<Tag> fetchRecursivelyByParentId(const int parentId);

    static bool isTaggingShowNotesRecursively();

    static QList<Tag> fetchAllOfNotes(const QList<Note>& notes);

protected:
    int id;
    int priority;
    int parentId;
    QColor _color;
    QString name;

    QString colorFieldName();

    static bool removeNoteLinkById(const int id);
};
