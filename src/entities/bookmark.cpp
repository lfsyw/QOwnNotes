/**
 * Bookmark class
 */

#include <utility>
#include "bookmark.h"
#include "notefolder.h"
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QJsonArray>
#include <QJsonDocument>


Bookmark::Bookmark() {
    url = "";
    name = "";
    tags = QStringList();
    description = "";
}

Bookmark::Bookmark(const QString &url, const QString &name, const QStringList &tagList,
        const QString &description) {
    this->url = url;
    this->name = name;
    this->tags = tagList;
    this->description = description;
};


QJsonObject Bookmark::jsonObject() const {
    QJsonObject bookmarkObject;
    bookmarkObject.insert("name", QJsonValue::fromVariant(name));
    bookmarkObject.insert("url", QJsonValue::fromVariant(url));
    bookmarkObject.insert("tags", QJsonValue::fromVariant(tags));
    bookmarkObject.insert("description", QJsonValue::fromVariant(description));
    return bookmarkObject;
};

QDebug operator<<(QDebug dbg, const Bookmark &bookmark) {
    dbg.nospace() << "Bookmark: <name>" << bookmark.name <<
                  " <url>" << bookmark.url << " <tags>" << bookmark.tags <<
                  " <description>" << bookmark.description;
    return dbg.space();
}

bool Bookmark::operator==(const Bookmark &bookmark) const {
    return url == bookmark.url;
}

/**
 * Parses bookmarks from a text
 *
 * @param text
 * @return
 */
QList<Bookmark> Bookmark::parseBookmarks(const QString& text, bool withBasicUrls) {
    QRegularExpressionMatchIterator i;
    QList<Bookmark> bookmarks;

    // parse bookmark links like `- [name](http://link) #tag1 #tag2 the description text`
    // with optional tags and description
    i = QRegularExpression(R"([-*] \[(.+?)\]\(([\w-]+://.+?)\)(.*)$)", QRegularExpression::MultilineOption).globalMatch(text);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString name = match.captured(1);
        QString url = match.captured(2);
        QString additionalText = match.captured(3);
        QStringList tags;
        QString description;

        if (!additionalText.isEmpty()) {

            QRegularExpressionMatchIterator addIterator =
                    QRegularExpression(R"(#([^\s#]+))").globalMatch(
                            additionalText);
            while (addIterator.hasNext()) {
                QRegularExpressionMatch addMatch = addIterator.next();
                QString tag = addMatch.captured(1).trimmed();

                if (!tags.contains(tag)) {
                    tags << tag;
                    additionalText.remove(QRegularExpression(
                            "#" + QRegularExpression::escape(tag) + "\\b"));
                }
            }

            description = additionalText.trimmed();
        }

        if (withBasicUrls && !tags.contains("current")) {
            tags << "current";
        }

        auto bookmark = Bookmark(url, name, tags, description);
        bookmark.mergeInList(bookmarks);
    }

    if (withBasicUrls) {
        // parse named links like [name](http://my.site.com)
        i = QRegularExpression(R"(\[(.+?)\]\(([\w-]+://.+?)\))").globalMatch(text);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString name = match.captured(1);
            QString url = match.captured(2);

            auto bookmark = Bookmark(url, name, QStringList() << "current");
            bookmark.mergeInList(bookmarks);
        }

        // parse links like <http://my.site.com>
        i = QRegularExpression(R"(<([\w-]+://.+?)>)").globalMatch(text);

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString url = match.captured(1);

            auto bookmark = Bookmark(url, "", QStringList() << "current");
            bookmark.mergeInList(bookmarks);
        }
    }

    return bookmarks;
}

/**
 * Returns json text of a bookmark list for the WebSocketServerService
 *
 * @return
 */
QString Bookmark::bookmarksWebServiceJsonText(const QList<Bookmark>& bookmarks) {
    QJsonArray bookmarkObjectList;
    QJsonArray noteFolderObjectList;

    Q_FOREACH(const Bookmark& bookmark, bookmarks) {
            bookmarkObjectList.push_back(bookmark.jsonObject());
        }

    Q_FOREACH(const NoteFolder& noteFolder, NoteFolder::fetchAll()) {
            noteFolderObjectList.push_back(noteFolder.jsonObject());
        }

    QJsonObject bookmarkResultObject;
    bookmarkResultObject.insert("type", QJsonValue::fromVariant("bookmarks"));
    bookmarkResultObject.insert("data", bookmarkObjectList);
    bookmarkResultObject.insert("noteFolderName",
            NoteFolder::currentNoteFolder().getName());
    bookmarkResultObject.insert("noteFolders", noteFolderObjectList);
    bookmarkResultObject.insert("noteFolderId",
            NoteFolder::currentNoteFolderId());

    QJsonDocument doc(bookmarkResultObject);

    return doc.toJson(QJsonDocument::Compact);
}

/**
 * Returns json text parsed from links of a text for the WebSocketServerService
 *
 * @return
 */
QString Bookmark::parsedBookmarksWebServiceJsonText(
        const QString& text, bool withBasicUrls) {
    return bookmarksWebServiceJsonText(parseBookmarks(
        text, withBasicUrls));
}

/**
 * Merges the current bookmark into a list of bookmarks
 */
void Bookmark::mergeInList(QList<Bookmark> &bookmarks) {
    int i = bookmarks.indexOf(*this);

    if (i == -1) {
        bookmarks.append(*this);
    } else {
        auto existingBookmark = bookmarks.at(i);

        // merge bookmarks
        existingBookmark.merge(*this);
        bookmarks[i] = existingBookmark;
    }
}

/**
 * Merges a bookmark into a list of bookmarks
 */
void Bookmark::mergeInList(QList<Bookmark> &bookmarks, Bookmark &bookmark) {
    bookmark.mergeInList(bookmarks);
}

/**
 * Merges the current bookmark with an other
 */
void Bookmark::merge(Bookmark &bookmark) {
    tags.append(bookmark.tags);
    tags.removeDuplicates();
    tags.sort();

    if (name.isEmpty()) {
        name = bookmark.name;
    }

    if (!description.contains(bookmark.description)) {
        if (!description.isEmpty()) {
            description += ", ";
        }

        description += bookmark.description;
    }
}

/**
 * Merges a list of bookmarks into another
 */
void Bookmark::mergeListInList(QList<Bookmark> &sourceBookmarks,
                               QList<Bookmark> &destinationBookmarks) {
    Q_FOREACH(Bookmark bookmark, sourceBookmarks) {
            bookmark.mergeInList(destinationBookmarks);
        }
}
