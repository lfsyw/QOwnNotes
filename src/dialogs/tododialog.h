#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QSplitter>

#include "entities/calendaritem.h"
#include "mainwindow.h"
#include "masterdialog.h"

namespace Ui {
class TodoDialog;
}

class MainWindow;

class TodoDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit TodoDialog(MainWindow *mainWindow, const QString &taskUid = "",
                            QWidget *parent = 0);
    ~TodoDialog();

    void reloadTodoListItems();
    void clearTodoList();
    void todoItemLoadingProgressBarIncrement();
    void todoItemLoadingProgressBarSetMaximum(int value);
    void todoItemLoadingProgressBarHide();
    void todoItemLoadingProgressBarHideIfOnMaximum();
    void jumpToTask(const QString &taskUid);
    void refreshUi();

public slots:
    void reloadTodoList();

private slots:
    void on_TodoDialog_finished(int result);
    void on_todoListSelector_currentIndexChanged(const QString &arg1);
    void on_todoList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_prioritySlider_valueChanged(int value);
    void on_showCompletedItemsCheckBox_clicked();
    void on_saveButton_clicked();
    void on_todoItemLoadingProgressBar_valueChanged(int value);
    void on_newItemEdit_returnPressed();
    void on_removeButton_clicked();
    void on_todoList_itemChanged(QListWidgetItem *item);
    void on_reminderCheckBox_clicked();
    void on_summaryEdit_returnPressed();
    void on_newItemEdit_textChanged(const QString &arg1);
    void onSaveAndInsertButtonClicked();
    void onImportAsNoteButtonClicked();
    void clearCacheAndReloadTodoList();

private:
    Ui::TodoDialog *ui;
    MainWindow *_mainWindow;
    QSplitter *mainSplitter;
    CalendarItem currentCalendarItem;
    CalendarItem lastCreatedCalendarItem;
    QString _jumpToCalendarItemUid;
    bool _setFocusToDescriptionEdit;
    int firstVisibleTodoListRow;
    void setupMainSplitter();
    void storeSettings();
    void loadTodoListData();
    int findTodoItemRowByUID(const QString &uid);
    void resetEditFrameControls();
    void setupUi();
    void updateCurrentCalendarItemWithFormData();
    void searchForSearchLineTextInNoteTextEdit();
    void searchInDescriptionTextEdit(QString &str);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

    void jumpToTodoListItem();
};

#endif // TODODIALOG_H
