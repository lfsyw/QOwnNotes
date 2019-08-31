#ifndef OWNCLOUDSERVICE_H
#define OWNCLOUDSERVICE_H

#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QObject>
#include <QXmlQuery>
#include <dialogs/sharedialog.h>
#include "mainwindow.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/tododialog.h"

#define QOWNNOTESAPI_MIN_VERSION "0.4.2"

// we set a user agent to prevent troubles with some ownCloud / Nextcloud
// server hosting providers
// see: https://github.com/pbek/QOwnNotes/issues/541
#define OWNCLOUD_SERVICE_USER_AGENT "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9a3pre) Gecko/20070330"

struct CalDAVCalendarData {
    QString url;
    QString displayName;
};

class SettingsDialog;
class MainWindow;
class TodoDialog;

class OwnCloudService : public QObject {
Q_OBJECT

public:
    enum CalendarBackend {
        LegacyOwnCloudCalendar = 0,
        CalendarPlus,
        CalDAVCalendar,
        DefaultOwnCloudCalendar
    };
    Q_ENUMS(CalendarBackend)

    explicit OwnCloudService(QObject *parent = 0);

    void settingsConnectionTest(SettingsDialog *dialog);

    void loadVersions(const QString& fileName, MainWindow *mainWindow);

    void loadTrash(MainWindow *mainWindow);

    void restoreTrashedNoteOnServer(const QString& fileName,
                                    int timestamp, MainWindow *mainWindow);

    void settingsGetCalendarList(SettingsDialog *dialog);

    void todoGetTodoList(const QString& calendarName, TodoDialog *dialog);

    void postCalendarItemToServer(CalendarItem calendarItem,
                                  TodoDialog *dialog);

    bool updateICSDataOfCalendarItem(CalendarItem *calItem);

    void removeCalendarItem(CalendarItem calItem, TodoDialog *dialog);

    void settingsGetFileList(SettingsDialog *dialog, const QString &path);

    static bool hasOwnCloudSettings(bool withEnabledCheck = true);

    void shareNote(Note note, ShareDialog *dialog);

    void fetchShares(const QString& path = "");

    void fetchBookmarks();

    void removeNoteShare(Note note, ShareDialog *dialog);

    static OwnCloudService *instance();

    static bool isOwnCloudSupportEnabled();

    static bool isTodoSupportEnabled();

    void startAppVersionTest();

    QString nextcloudPreviewImageTagToInlineImageTag(QString imageTag);

private:
    QString serverUrl;
    QString todoCalendarServerUrl;
    QString serverUrlPath;
    QString todoCalendarServerUrlPath;
    QString serverUrlWithoutPath;
    QString todoCalendarServerUrlWithoutPath;
    QString userName;
    QString todoCalendarUsername;
    QString password;
    QString todoCalendarPassword;
    QNetworkAccessManager *networkManager;
    QNetworkAccessManager *calendarNetworkManager;
    MainWindow *mainWindow;
    ShareDialog *shareDialog;
    static const QString rootPath;
    static const QString format;
    QString versionListPath;
    QString trashListPath;
    QString appInfoPath;
    QString capabilitiesPath;
    QString ownCloudTestPath;
    QString restoreTrashedNotePath;
    QString webdavPath;
    QString sharePath;
    QString bookmarkPath;
    SettingsDialog *settingsDialog;
    TodoDialog *todoDialog;
    QString calendarName;

    void checkAppInfo(QNetworkReply *reply);

    void readSettings();

    void addAuthHeader(QNetworkRequest *r);

    void addCalendarAuthHeader(QNetworkRequest *r);

    void handleVersionsLoading(const QString &data);

    void handleTrashedLoading(const QString &data);

    QList<CalDAVCalendarData> parseCalendarData(QString &data);

    void loadTodoItems(QString &data);

    static void ignoreSslErrorsIfAllowed(QNetworkReply *reply);

    void loadDirectory(QString &data);

    void showOwnCloudServerErrorMessage(
            const QString& message = QString(""), bool withSettingsButton = true);

    void showOwnCloudMessage(
            const QString &headline = QString(""), const QString &message = QString(""),
            bool withSettingsButton = true);

    void updateNoteShareStatusFromShare(QString &data);

    void updateNoteShareStatusFromFetchAll(QString &data);

    void handleNoteShareReply(QString &data);

    void updateNoteShareStatus(QXmlQuery &query,
                               bool updateShareDialog = false);

    void handleDeleteNoteShareReply(const QString &urlPart, QString &data);

    static void checkAppVersion(QNetworkReply *reply);

    void handleImportBookmarksReply(QString &data);

    QByteArray downloadNextcloudPreviewImage(const QString &path);

signals:

private slots:

    void slotAuthenticationRequired(QNetworkReply *reply,
                                    QAuthenticator *authenticator);

    void slotCalendarAuthenticationRequired(QNetworkReply *reply,
                                            QAuthenticator *authenticator);

    void slotReplyFinished(QNetworkReply *);
};

#endif // OWNCLOUDSERVICE_H
