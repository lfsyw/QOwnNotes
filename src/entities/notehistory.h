#ifndef NOTEHISTORY_H
#define NOTEHISTORY_H

#include <QPlainTextEdit>
#include <QDataStream>
#include "note.h"


class NoteHistoryItem
{
public:
    explicit NoteHistoryItem(const Note *note = nullptr, QPlainTextEdit *textEdit = nullptr);
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
    bool loopable = false;

public:
    void setLoopable(bool loop) { loopable = loop; }
    int lastIndex() const;

public:
    NoteHistory();
    //copy
    NoteHistory(const NoteHistory&);
    NoteHistory &operator=(const NoteHistory& rhs);
    //move
    NoteHistory(NoteHistory&&) noexcept;
    NoteHistory &operator=(NoteHistory&& rhs) noexcept;
    ~NoteHistory();

    void add(Note note, QPlainTextEdit *textEdit);
    friend QDebug operator<<(QDebug dbg, const NoteHistory &history);
    bool back();
    bool forward();
    bool isEmpty() const;
    NoteHistoryItem getCurrentHistoryItem() const;
    void updateCursorPositionOfNote(const Note &note, QPlainTextEdit *textEdit);
    void clear();
    NoteHistoryItem getLastItemOfNote(const Note &note) const;
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
