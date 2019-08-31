#pragma once

#include <QSqlQuery>
#include <QStringList>
#include "notesubfolder.h"

class NoteFolder
{
public:
    explicit NoteFolder();

    int getId();
    static bool create(const QString &name, const QString &localPath,
                       int ownCloudServerId, const QString &remotePath);
    static NoteFolder fetch(int id);
    static NoteFolder noteFolderFromQuery(const QSqlQuery& query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const NoteFolder &noteFolder);
    bool exists();
    bool fillFromQuery(const QSqlQuery& query);
    bool remove();
    bool isFetched();
    static QList<NoteFolder> fetchAll();
    QString getName();
    int getOwnCloudServerId();
    QString getLocalPath();
    QString getRemotePath();
    int getPriority();
    void setName(const QString &text);
    void setLocalPath(const QString &text);
    void setPriority(int value);
    void setOwnCloudServerId(int id);
    void setRemotePath(const QString &text);
    static int countAll();
    static bool migrateToNoteFolders();
    void setAsCurrent();
    bool isCurrent();
    static int currentNoteFolderId();
    static NoteFolder currentNoteFolder();
    bool localPathExists();
    QString suggestRemotePath();
    QString fixRemotePath();
    static QString currentRemotePath(bool addTrailingSlash = true);
    static QString currentLocalPath();
    static QString currentRootFolderName(bool fullPath = false);
    void setActiveTagId(int value);
    int getActiveTagId();
    bool isShowSubfolders();
    void setShowSubfolders(bool value);
    static bool isCurrentShowSubfolders();
    void setActiveNoteSubFolder(NoteSubFolder noteSubFolder);
    NoteSubFolder getActiveNoteSubFolder();
    void resetActiveNoteSubFolder();
    static QString currentTrashPath();
    static QString currentMediaPath();
    static QString currentAttachmentsPath();
    bool isUseGit();
    void setUseGit(bool value);
    QJsonObject jsonObject() const;
    static QString noteFoldersWebServiceJsonText();

private:
    int id;
    QString name;
    QString localPath;
    int ownCloudServerId;
    QString remotePath;
    int priority;
    int activeTagId;
    QString activeNoteSubFolderData;
    bool showSubfolders;
    bool useGit;
};
