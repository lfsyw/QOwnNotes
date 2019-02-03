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

    int getId();

    static Tag fetch(int id);

    static Tag tagFromQuery(QSqlQuery query);

    bool store();

    friend QDebug operator<<(QDebug dbg, const Tag &tag);

    bool exists();

    bool fillFromQuery(QSqlQuery query);

    bool remove();

    bool isFetched();

    static QList<Tag> fetchAll();

    QString getName();

    int getPriority();

    void setName(const QString &text);

    void setPriority(int value);

    static int countAll();

    void setAsActive();

    bool isActive();

    static int activeTagId();

    static Tag activeTag();

    static Tag fetchByName(const QString &name, bool startsWith = false);

    static Tag fetchByName(const QString &name, int parentId);

    bool linkToNote(const Note &note);

    static QList<Tag> fetchAllOfNote(const Note &note);

    bool removeLinkToNote(const Note &note);

    QStringList fetchAllLinkedNoteFileNames(bool fromAllSubfolders);

    QList<Note> fetchAllLinkedNotes();

    static QStringList fetchAllNames();

    bool isLinkedToNote(const Note &note);

    static bool removeAllLinksToNote(const Note &note);

    static bool renameNoteFileNamesOfLinks(
            const QString &oldFileName, const QString &newFileName);

    int countLinkedNoteFileNames(bool fromAllSubfolder, bool recursive);

    static QList<Tag> fetchAllWithLinkToNoteNames(const QStringList &noteNameList);

    int getParentId();

    void setParentId(int id);

    static QList<Tag> fetchAllByParentId(int parentId, const QString &sortBy = "created DESC");

    static int countAllParentId(int parentId);

    bool hasChild(int tagId);

    static int countAllOfNote(const Note &note);

    static void setAsActive(int tagId);

    static void convertDirSeparator();

    QColor getColor();

    void setColor(QColor color);

    static Tag fetchOneOfNoteWithColor(const Note &note);

    static void migrateDarkColors();

    static void removeBrokenLinks();

    static QStringList fetchAllNamesOfNote(const Note &note);

    static QStringList searchAllNamesByName(const QString &name);

    static QList<Tag> fetchRecursivelyByParentId(int parentId);

    static bool isTaggingShowNotesRecursively();

    static QList<Tag> fetchAllOfNotes(QList<Note> notes);

    bool operator ==(const Tag &tag) const;

    bool operator <(const Tag &tag) const;

protected:
    int id;
    QString name;
    int priority;
    int parentId;
    QColor _color;

    QString colorFieldName();

    static bool removeNoteLinkById(int id);
};
