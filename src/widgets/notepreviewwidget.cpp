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
        }

        return false;
    }

    return QTextBrowser::eventFilter(obj, event);
}

/**
 * @brief Extract local gif urls from html
 * @param text
 * @return Urls to gif files
 */
QStringList NotePreviewWidget::extractGifUrls(const QString &text) const {
    static QRegExp regex(R"(<img[^>]+src=\"(file:\/\/\/[^\"]+\.gif)\")", Qt::CaseInsensitive);

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
    while (true)
    {
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
        auto* movie = new QMovie(this);
        movie->setFileName(QUrl(url).toLocalFile());
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

QString NotePreviewWidget::handleTaskLists(const QString &text_) {
    //TODO
    // to ensure the clicking behavior of checkboxes,
    // line numbers of checkboxes in the original markdown text
    // should be provided by the markdown parser
    auto text = text_;

    const QString checkboxStart = R"(<a class="task-list-item-checkbox" href="checkbox://_)";
    text.replace(QRegExp(R"((<li>\s*(<p>)*\s*)\[ ?\])", Qt::CaseInsensitive), "\\1" + checkboxStart + "\">&#9744;</a>");
    text.replace(QRegExp(R"((<li>\s*(<p>)*\s*)\[[xX]\])", Qt::CaseInsensitive), "\\1" + checkboxStart + "\">&#9745;</a>");

    int count = 0;
    int pos = 0;
    while (true) {
        pos = text.indexOf(checkboxStart + "\"", pos);
        if (pos == -1)
            break;

        pos += checkboxStart.length();
        text.insert(pos, QString::number(count++));
    }

    return text;
}

QString NotePreviewWidget::handleLocalImageLinks(const QString &text_)
{
    QString text;

    QRegExp imgRegex(R"(<img[^>]+src=\"(file:\/\/\/[^\"]+)\".*\/>)", Qt::CaseInsensitive);
    imgRegex.setMinimal(true);

    // avoid genarating <a ...><img ... /></a> into <a ...><a ...><img ... /></a></a>
    QRegExp linkRegex(R"(<a[^>]+>)", Qt::CaseInsensitive);
    linkRegex.setMinimal(true);

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
            text += QString(R"(<a href="%1">%2</a>)").arg(imgRegex.cap(1), imgRegex.cap(0));
        } else {
            text += imgRegex.cap(0);
        }
        i = imgPos + imgRegex.matchedLength();
    }

    return text;
}

void NotePreviewWidget::setHtml(const QString &text) {
    animateGif(text);

    _html = handleLocalImageLinks(handleTaskLists(text));
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
            connect(watcher, SIGNAL(finished()), this, SLOT(updateOnlineMedia()));
            connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));

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
    // use m_html instead of toHtml(), because the content
    // returned by Qt has been heavily tuned.
    if (sender()) {
        auto watcher = static_cast<QFutureWatcher<QPair<QString, QString>>*>(sender());
        auto result = watcher->result();
        if (!_url2media.contains(result.first)) {
            _url2media[result.first] = result.second;
        }
    }

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
