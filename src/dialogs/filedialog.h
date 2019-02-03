#ifndef FILEDIALOG_H
#define FILEDIALOG_H


#include <QtWidgets/QFileDialog>

class FileDialog : public QFileDialog
{
Q_OBJECT

public:
    explicit FileDialog(const QString &name = "");
    QString selectedFile();

private:
    QString _settingsKey;
    QString _generalSettingsKey;

protected slots:
    void storeDirectory(const QString &path);
    void storeDirectory(const QStringList &files);
};

#endif // FILEDIALOG_H