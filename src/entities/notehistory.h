#ifndef NOTEHISTORY_H
#define NOTEHISTORY_H

#include <QPlainTextEdit>
#include <QDataStream>
#include "note.h"


class NoteHistoryItem
{
public:
    explicit NoteHistoryItem(const Note *note = NULL, QPlainTextEdit *textEdit = NULL);
    explicit NoteHistoryItem(const QString &noteName, const QString &noteSubFolderPathData,
                             int cursorPosition,
                             float relativeScrollBarPosition);
    friend QDebug operator<<(QDebug dbg, const NoteHistoryItem &item);
    QString getNoteName() const;
    QString getNoteSubFolderPathData() const;
    int getCursorPosition() const;
    float getRelativeScrollBarPosition() const;
    Note getNote() const;
    bool isNoteValid() const;
    bool operator ==(const NoteHistoryItem &item) const;
    void restoreTextEditPosition(QPlainTextEdit *textEdit) const;

private:
    QString _noteName;
    QString _noteSubFolderPathData;
    int _cursorPosition;
    float _relativeScrollBarPosition;
    static float getTextEditScrollBarRelativePosition(QPlainTextEdit *textEdit);
};

// we want to store the class to the settings
QDataStream &operator<<(QDataStream &out, const NoteHistoryItem &item);
QDataStream &operator>>(QDataStream &in, NoteHistoryItem &item);
Q_DECLARE_METATYPE(NoteHistoryItem)

class NoteHistory
{
private:
    QList<NoteHistoryItem> noteHistory;
    int currentIndex;
    NoteHistoryItem currentHistoryItem;
    bool loopable;
    int lastIndex();

public:
    explicit NoteHistory();
    void setLoopable(bool loop) { loopable = loop; }
    void add(const Note &note, QPlainTextEdit *textEdit);
    friend QDebug operator<<(QDebug dbg, const NoteHistory &history);
    bool back();
    bool forward();
    bool isEmpty();
    NoteHistoryItem getCurrentHistoryItem();
    void updateCursorPositionOfNote(const Note &note, QPlainTextEdit *textEdit);
    void clear();
    NoteHistoryItem getLastItemOfNote(const Note &note);
    QList<NoteHistoryItem> getNoteHistoryItems() const;
    void addNoteHistoryItem(NoteHistoryItem item);
    void storeForCurrentNoteFolder();
    void restoreForCurrentNoteFolder();
};

// we want to store the class to the settings
QDataStream &operator<<(QDataStream &out, const NoteHistory &history);
QDataStream &operator>>(QDataStream &in, NoteHistory &history);
Q_DECLARE_METATYPE(NoteHistory)

#endif // NOTEHISTORY_H
