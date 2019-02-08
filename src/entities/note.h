#ifndef NOTE_H
#define NOTE_H

#include <QDateTime>
#include <QSqlQuery>
#include <QFile>
#include <QUrl>
#include <QRegularExpression>
#include "notesubfolder.h"
#include "bookmark.h"

#define NOTE_TEXT_ENCRYPTION_PRE_STRING "<!-- BEGIN ENCRYPTED TEXT --"
#define NOTE_TEXT_ENCRYPTION_POST_STRING "-- END ENCRYPTED TEXT -->"
#define BOTAN_SALT "Gj3%36/SmPoe12$snNAs-A-_.),?faQ1@!f32"

class Note {
public:
    explicit Note();

    int getId() const;

    QString getName() const;

    QString getFileName() const;

    QString getNoteText() const;

    bool getHasDirtyData() const;

    void setHasDirtyData(bool hasDirtyData);

    void setName(const QString &text);

    void setNoteText(const QString &text);

    qint64 getCryptoKey() const;

    QString getCryptoPassword() const;

    void setCryptoKey(qint64 cryptoKey);

    static bool addNote(const QString &name, const QString &fileName, const QString &text);

    static Note fetch(int id);

    static Note fetchByName(QRegularExpression regExp,
                            int noteSubFolderId = -1);

    static Note fetchByFileName(const QString &fileName, int noteSubFolderId = -1);

    static Note fetchByName(const QString &name, int noteSubFolderId = -1);

    static QList<Note> fetchAll(int limit = -1);

    static QList<Note> fetchAllNotTagged(int activeNoteSubFolderId);

    static QStringList fetchAllNotTaggedNames();

    static int countAllNotTagged(int activeNoteSubFolderId=-1);

    static QList<Note> search(const QString &text);

    static QList<QString> searchAsNameListInCurrentNoteSubFolder(
            const QString &text, bool searchInNameOnly = false);

    static QList<QString> searchAsNameList(
            const QString &text, bool searchInNameOnly = false);

    static QStringList fetchNoteNamesInCurrentNoteSubFolder();

    static QStringList fetchNoteNames();

    static QStringList fetchNoteFileNames();

    static Note noteFromQuery(QSqlQuery query);

    bool store();

    bool storeNewText(const QString &text);

    bool storeNoteTextFileToDisk();

    static QString defaultNoteFileExtension();

    static QStringList customNoteFileExtensionList(const QString &prefix = "");

    static QString getFullNoteFilePathForFile(const QString &fileName);

    static int storeDirtyNotesToDisk(Note &currentNote,
                                     bool *currentNoteChanged = Q_NULLPTR,
                                     bool *noteWasRenamed = Q_NULLPTR);

    bool updateNoteTextFromDisk();

    friend QDebug operator<<(QDebug dbg, const Note &note);

    void createFromFile(QFile &file, int noteSubFolderId = 0,
                        bool withNoteNameHook = false);

    static bool deleteAll();

    bool fileExists() const;

    bool fileWriteable() const;

    bool exists() const;

    bool refetch();

    bool fillFromQuery(QSqlQuery query);

    bool fillByFileName(const QString &fileName, int noteSubFolderId = -1);

    bool removeNoteFile();

    bool remove(bool withFile = false);

    QString toMarkdownHtml(const QString &notesPath, int maxImageWidth = 980,
                           bool forExport = false, bool decrypt = true,
                           bool base64Images = false);

    bool isFetched();

    bool copyToPath(const QString &destinationPath);

    bool moveToPath(const QString &destinationPath);

    static QString generateTextForLink(const QString &text);

    static qint64 qint64Hash(const QString &str);

    QString encryptNoteText();

    QString getDecryptedNoteText() const;

    bool hasEncryptedNoteText() const;

    void setCryptoPassword(const QString &password);

    bool canDecryptNoteText() const;

    static bool expireCryptoKeys();

    QUrl fullNoteFileUrl() const;

    QString fullNoteFilePath() const;

    static QString encodeCssFont(const QFont& refFont);

    void setDecryptedNoteText(const QString &text);

    bool storeNewDecryptedText(const QString &text);

    QDateTime getFileLastModified() const;

    QDateTime getFileCreated() const;

    QDateTime getModified() const;

    static int countAll();

    static bool allowDifferentFileName();

    bool renameNoteFile(const QString &newName);

    QString fileNameSuffix();

    bool modifyNoteTextFileNameFromQMLHook();

    static QList<int> searchInNotes(const QString &query,
                                    bool ignoreNoteSubFolder = false,
                                    int noteSubFolderId = -1);

    static QStringList buildQueryStringList(
            const QString &searchString, bool escapeForRegularExpression = false);

    QString fileBaseName(bool withFullName = false);

    NoteSubFolder getNoteSubFolder() const;

    void setNoteSubFolder(NoteSubFolder noteSubFolder);

    void setNoteSubFolderId(int id);

    static QList<Note> fetchAllByNoteSubFolderId(int noteSubFolderId);

    static QList<int> noteIdListFromNoteList(QList<Note> noteList);

    static int countByNoteSubFolderId(int noteSubFolderId = 0);

    int getNoteSubFolderId() const;

    bool isInCurrentNoteSubFolder();

    QString relativeNoteFilePath(const QString &separator = "") const;

    QString relativeNoteSubFolderPath() const;

    QString noteSubFolderPathData() const;

    bool isSameFile(const Note &note) const;

    QString getShareUrl() const;

    void setShareUrl(const QString &url);

    int getShareId() const;

    void setShareId(int id);

    bool isShared() const;

    static Note fetchByShareId(int shareId);

    qint64 getFileSize() const;

    static Note updateOrCreateFromFile(QFile &file, NoteSubFolder noteSubFolder,
                                           bool withNoteNameHook = false);

    static QList<int> fetchAllIds(int limit = -1, int offset = -1);

    static QList<int> findLinkedNotes(const QString &fileName);

    static void handleNoteRenaming(const QString &oldFileName, const QString &newFileName);

    static QString createNoteHeader(const QString &name);

    static QString getInsertMediaMarkdown(QFile *file, const QString &identifier = QString(), bool addNewLine = true,
                                          bool returnUrlOnly = false);

    static QString getHashForString(const QString &str);

    static QString getInsertAttachmentMarkdown(QFile *file,
                                               const QString &fileName = "",
                                               bool returnUrlOnly= false);

    static bool scaleDownImageFileIfNeeded(QFile &file);

    static QString downloadUrlToMedia(const QUrl &url, bool returnUrlOnly = false);
    static QString getUrlMedia(const QUrl &url);
    static QString getUrlMediaSuffix(const QUrl &url);
    static QStringList getMediaExtensions();

    bool canWriteToNoteFile() const;

    static QString generateNoteFileNameFromName(const QString &name);

    void generateFileNameFromName();

    QString textToMarkdownHtml(const QString &str, const QString &notesPath,
                               int maxImageWidth = 980, bool forExport = false,
                               bool base64Images = false);

    QStringList getMediaFileList() const;

    static Note fetchByUrlString(const QString &urlString);

    static QList<int> fetchAllIdsByNoteTextPart(const QString &textPart);

    QStringList getAttachmentsFileList() const;

    QString getNotePreviewText(bool asHtml = false, int lines = 3) const;

    static QString generateMultipleNotesPreviewText(QList<Note> notes);

    bool handleNoteTextFileName();

    QString getNoteIdURL();

    static QString cleanupFileName(const QString &name);

    static QString extendedCleanupFileName(const QString &name);

    QList<Bookmark> getParsedBookmarks() const;

    QString getParsedBookmarksWebServiceJsonText() const;

    void resetNoteTextHtmlConversionHash();

protected:
    int id;
    QString name;
    QString fileName;
    qint64 fileSize;
    int noteSubFolderId;
    QString noteText;
    QString decryptedNoteText;
    bool hasDirtyData;
    QDateTime fileCreated;
    QDateTime fileLastModified;
    QDateTime created;
    QDateTime modified;
    qint64 cryptoKey;
    QString cryptoPassword;
    QString shareUrl;
    int shareId;
    QRegularExpression getEncryptedNoteTextRegularExpression() const;
    QString getEncryptedNoteText() const;
    QString _noteTextHtml;
    QString _noteTextHtmlConversionHash;

signals:

public slots:

    static const QString getNoteURL(const QString &baseName);

    static const QString getNoteURLFromFileName(const QString &fileName);
};

#endif // NOTE_H
