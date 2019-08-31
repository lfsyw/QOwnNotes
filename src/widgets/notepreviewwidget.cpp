/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "notepreviewwidget.h"
#include "entities/note.h"
#include "entities/notefolder.h"
#include "mainwindow.h"
#include "utils/misc.h"
#include <QLayout>
#include <QDebug>
#include <QRegExp>
#include <QMovie>
#include <QtConcurrent>
#include <QProxyStyle>

class NoDottedOutlineForLinksStyle: public QProxyStyle {
public:
    int styleHint(StyleHint hint,
                  const QStyleOption *option,
                  const QWidget *widget,
                  QStyleHintReturn *returnData) const Q_DECL_OVERRIDE {
        if (hint == SH_TextControl_FocusIndicatorTextCharFormat)
            return 0;
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

static bool checkFileHead(const QString &fileName, const QString &identifier)
{
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly))
        return f.peek(identifier.length()) == identifier;
    else
        return false;
}

NotePreviewWidget::NotePreviewWidget(QWidget *parent) : QTextBrowser(parent) {
    // add the hidden search widget
    _searchWidget = new QTextEditSearchWidget(this);
    _searchWidget->setReplaceEnabled(false);

    // add a layout to the widget
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setMargin(0);
    layout->addStretch();
    this->setLayout(layout);
    this->layout()->addWidget(_searchWidget);

    installEventFilter(this);
    viewport()->installEventFilter(this);

    auto proxyStyle = new NoDottedOutlineForLinksStyle;
    proxyStyle->setParent(this);
    setStyle(proxyStyle);

    QFont font;
    font.fromString(QSettings().value("MainWindow/noteTextView.code.font").toString());
    setTabStopWidth(4 * QFontMetrics(font).width(' '));
}

void NotePreviewWidget::resizeEvent(QResizeEvent* event) {
    emit resize(event->size(), event->oldSize());

    // we need this, otherwise the preview is always blank
    QTextBrowser::resizeEvent(event);
}

bool NotePreviewWidget::eventFilter(QObject *obj, QEvent *event) {
//    qDebug() << event->type();
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // disallow keys if widget hasn't focus
        if (!this->hasFocus()) {
            return true;
        }

        if ((keyEvent->key() == Qt::Key_Escape) && _searchWidget->isVisible()) {
            _searchWidget->deactivate();
            return true;
        }  else if ((keyEvent->key() == Qt::Key_F) &&
                   keyEvent->modifiers().testFlag(Qt::ControlModifier)) {
            _searchWidget->activate();
            return true;
        } else if ((keyEvent->key() == Qt::Key_F3)) {
            _searchWidget->doSearch(
                    !keyEvent->modifiers().testFlag(Qt::ShiftModifier));
            return true;
        } else if (keyEvent == QKeySequence::Copy) {
            auto cursor = textCursor();
            if (cursor.selectedText() == "\357\277\274") { // the 'obj' symbol
                QTextFormat format = cursor.charFormat();
                if (!format.isImageFormat()) {
                    cursor.setPosition(cursor.position() + 1);
                    format = cursor.charFormat();
                }
                if (format.isImageFormat()) {
                    auto imagePath = format.toImageFormat().name();
                    QUrl imageUrl = QUrl(imagePath);
                    if (imageUrl.isLocalFile()) {
                        Utils::Misc::copyImage(imageUrl.toLocalFile());
                        return true;
                    }
                }
            }
        }

        return false;
    }

    return QTextBrowser::eventFilter(obj, event);
}

void NotePreviewWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    static QString chineasePunctuations =
        "\343\200\202\357\274\237\357\274\201\357\274\214"
        "\343\200\201\357\274\233\357\274\232\342\200\234"
        "\342\200\235\342\200\230\342\200\231\357\274\210"
        "\357\274\211\343\200\212\343\200\213\343\200\210"
        "\343\200\211\343\200\220\343\200\221\343\200\216"
        "\343\200\217\343\200\214\343\200\215\357\271\203"
        "\357\271\204\343\200\224\343\200\225\342\200\246"
        "\342\200\224\357\275\236\357\271\217\357\277\245";
    static QString punctuations = R"(.,/#!$%\^&\*;:{}=\-_`~())";

    auto oldPos = textCursor().position();
    QTextBrowser::mouseDoubleClickEvent(event);

    auto cursor = textCursor();
    if (!cursor.hasSelection()) {
        if (cursor.atBlockEnd()) {
            auto blockText = cursor.block().text();
            if (!blockText.isEmpty()) {
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
                auto text = cursor.selectedText();
                int n = blockText.length();
                int sameCharCount = 1;
                if (text[0].isSpace()) {
                    while (sameCharCount < n && blockText[n - 1 - sameCharCount].isSpace())
                        sameCharCount++;
                }
                else if (punctuations.contains(text)) {
                    while (sameCharCount < n && punctuations.contains(QChar(blockText[n - 1 - sameCharCount])))
                        sameCharCount++;
                }
                cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, sameCharCount - 1);
                setTextCursor(cursor);
            }
        }
        else if (cursor.atBlockStart()) {
            auto blockText = cursor.block().text();
            if (!blockText.isEmpty()) {
                int spaceCount = 0;
                while (spaceCount < blockText.length() && blockText[spaceCount].isSpace())
                    ++spaceCount;
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, spaceCount);
                setTextCursor(cursor);
            }
        }
        return;
    }
    
    auto selStart = cursor.selectionStart();
    auto selEnd = cursor.selectionEnd();

    // cursor at spaces
    if (selEnd < oldPos) {
        auto blockText = cursor.block().text();
        cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
        selStart = selEnd = oldPos - cursor.position();
        while (selStart >= 1 && blockText[selStart - 1].isSpace())
            --selStart;
        while (selEnd < blockText.length() && blockText[selEnd].isSpace())
            ++selEnd;
        selStart += cursor.position();
        selEnd += cursor.position();
        cursor.setPosition(selStart);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, selEnd - selStart);
        setTextCursor(cursor);
        return;
    }

    auto text = cursor.selectedText();
    oldPos -= selStart;
    
    QRegExp re("[" + chineasePunctuations + "]+");
    auto index = text.lastIndexOf(re, oldPos);
    bool punctuationUnderCursor = false;
    if (index != -1) {
        if (index == oldPos || (cursor.atBlockEnd() && index == oldPos - 1)) {
            index = text.lastIndexOf(QRegExp("[^" + chineasePunctuations + "]"), oldPos);
            punctuationUnderCursor = true;
        }
        selStart += index + 1;
    }
    index = text.indexOf(re, oldPos);
    if (index != -1) {
        if (!punctuationUnderCursor)
            selEnd -= text.length() - index;
        else
            selEnd -= text.length() - (index + re.matchedLength());
    }
    
    if (selEnd - selStart == text.length())
        return;

    cursor.setPosition(selStart);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, selEnd - selStart);
    setTextCursor(cursor);
}

/**
 * @brief Extract local gif urls from html
 * @param text
 * @return Urls to gif files
 */
QStringList NotePreviewWidget::extractGifUrls(const QString &text) const {
    static QRegExp regex(R"(<img[^>]+src=\"(file:\/\/\/[^\"]+\.(gif|png))\")", Qt::CaseInsensitive);

    QStringList urls;
    int pos = 0;
    while (true) {
        pos = regex.indexIn(text, pos);
        if (pos == -1)
            break;
        QString url = regex.cap(1);
        if (!urls.contains(url))
            urls.append(url);
        pos += regex.matchedLength();
    }

    return urls;
}

QStringList NotePreviewWidget::extractHttpImageUrls(const QString &text) const
{
    static QRegExp regex(R"(<img[^>]+src=\"(http[^\"]+)\")", Qt::CaseInsensitive);

    QStringList urls;
    int pos = 0;
    while (true) {
        pos = regex.indexIn(text, pos);
        if (pos == -1)
            break;
        QString url = regex.cap(1);
        if (!urls.contains(url))
            urls.append(url);
        pos += regex.matchedLength();
    }

    return urls;
}

/**
 * @brief Setup animations for gif
 * @return
 */
void NotePreviewWidget::animateGif(const QString &text) {
    // clear resources
    if (QTextDocument* doc = document())
        doc->clear();

    QStringList urls = extractGifUrls(text);

    for (QMovie* &movie : _movies) {
        QString url = movie->property("URL").toString();
        if (urls.contains(url))
            urls.removeAll(url);
        else {
            movie->deleteLater();
            movie = nullptr;
        }
    }
    _movies.removeAll(nullptr);

    for (const QString &url : urls) {
        QString localFn = QUrl(url).toLocalFile();
        if (checkFileHead(localFn, "gif"))
            continue;

        auto* movie = new QMovie(this);
        movie->setFileName(localFn);
        movie->setCacheMode(QMovie::CacheNone);

        if (!movie->isValid() || movie->frameCount() < 2) {
            movie->deleteLater();
            continue;
        }

        movie->setProperty("URL", url);
        _movies.append(movie);

        connect(movie, &QMovie::frameChanged,
                this, [this, url, movie](int) {
            if (auto doc = document()) {
                doc->addResource(QTextDocument::ImageResource, url, movie->currentPixmap());
                doc->markContentsDirty(0, doc->characterCount());
            }
        });

        movie->start();
    }
}

QString NotePreviewWidget::handleLocalImageLinks(const QString &text_)
{
    QString text;

    QRegExp imgRegex(R"(<img[^>]+(src=\"(file:\/\/\/[^\"]+)\".*\/?>))", Qt::CaseInsensitive);
    imgRegex.setMinimal(true);

    // avoid genarating <a ...><img ... /></a> into <a ...><a ...><img ... /></a></a>
    QRegExp linkRegex(R"(<a[^>]+>)", Qt::CaseInsensitive);
    linkRegex.setMinimal(true);

    QMargins margins = contentsMargins();
    const int maxImageWidth = viewport()->width() - margins.left() - margins.right() - 15;

    int i = 0;
    while (true)
    {
        auto imgPos = imgRegex.indexIn(text_, i);
        if (imgPos == -1)
        {
            text += text_.mid(i);
            break;
        }
        text += text_.mid(i, imgPos - i);
        auto linkPos = text_.lastIndexOf(linkRegex, imgPos);
        if (linkPos == -1 || linkPos + linkRegex.matchedLength() != imgPos) {
            // cap the image width at maxImageWidth (note text view width)
            QString fileName = QUrl(imgRegex.cap(2)).toLocalFile();
            QImageReader imageReader(fileName);
            // some images may have wrong suffixes
            imageReader.setDecideFormatFromContent(true);
            text += QString(R"(<a href="%1"><img width="%2" %3</a>)")
                .arg(imgRegex.cap(2))
                .arg(qMin(imageReader.size().width(), maxImageWidth))
                .arg(imgRegex.cap(1));
        } else {
            text += imgRegex.cap(0);
        }
        i = imgPos + imgRegex.matchedLength();
    }

    return text;
}

void NotePreviewWidget::setHtml(const QString &text) {
    animateGif(text);

    _html = handleLocalImageLinks(Utils::Misc::parseTaskList(text, true));
    QTextBrowser::setHtml(_html);
    //qInfo() << _html;

    downloadOnlineMedia();
}

/**
 * @brief Returns the searchWidget instance
 * @return
 */
QTextEditSearchWidget *NotePreviewWidget::searchWidget() {
    return _searchWidget;
}

/**
 * Uses an other widget as parent for the search widget
 */
void NotePreviewWidget::initSearchFrame(QWidget *searchFrame, bool darkMode) {
    _searchFrame = searchFrame;

    // remove the search widget from our layout
    layout()->removeWidget(_searchWidget);

    QLayout *layout = _searchFrame->layout();

    // create a grid layout for the frame and add the search widget to it
    if (layout == NULL) {
        layout = new QVBoxLayout();
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
    }

    _searchWidget->setDarkMode(darkMode);
    _searchWidget->setReplaceEnabled(false);
    layout->addWidget(_searchWidget);
    _searchFrame->setLayout(layout);
}

/**
 * Hides the preview and the search widget
 */
void NotePreviewWidget::hide() {
    _searchWidget->hide();
    QWidget::hide();
}

void NotePreviewWidget::downloadOnlineMedia() {
    for (const auto &url : extractHttpImageUrls(_html)) {
        if (!_url2media.contains(url)) {
            auto mediaPath = Note::getUrlMedia(url);
            if (!mediaPath.isEmpty()) {
                _url2media[url] = mediaPath;
                continue;
            }

            auto watcher = new QFutureWatcher<QPair<QString, QString>>();
            connect(watcher, SIGNAL(finished()), this, SLOT(updateOnlineMediaFromFutureWatcher()));
            connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));

            MainWindow::instance()->showStatusBarMessage(tr("Downloading %1").arg(url), 5000);
            
            auto future = QtConcurrent::run(
                [url]{
                return qMakePair(url, Note::downloadUrlToMedia(url, true));
            });
            watcher->setFuture(future);
            return; // download them one by one in order
        }
    }
    updateOnlineMedia();
}

void NotePreviewWidget::updateOnlineMedia()
{
    // use _html instead of toHtml(), because the content
    // returned by Qt has been heavily tuned.
    auto html = _html;

    bool changed = false;
    bool allDownloaded = true;
    for (const auto &url : extractHttpImageUrls(html)) {
        if (!_url2media.contains(url)) {
            allDownloaded = false;
        }
        else {
            QString mediaFilePath = _url2media[url];
            if (!mediaFilePath.isEmpty()) {
                mediaFilePath.replace("file://media/",
                                      "file:///" + NoteFolder::currentNoteFolder().getLocalPath() + "/media/");
                html.replace("src=\"" + url, "src=\"" + mediaFilePath);
                changed = true;
            }
        }
    }

    if (changed)
        setHtml(html);
    else if (!allDownloaded)
        downloadOnlineMedia();
}

void NotePreviewWidget::updateOnlineMediaFromFutureWatcher()
{
    if (sender()) {
        auto watcher = static_cast<QFutureWatcher<QPair<QString, QString>>*>(sender());
        auto result = watcher->result();
        if (!_url2media.contains(result.first)) {
            _url2media[result.first] = result.second;
        }
    }

    updateOnlineMedia();
}
