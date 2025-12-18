#pragma once

#include <QDialog>
#include <QLineEdit>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void saveSettings();

private:
    QLineEdit* m_txtExifToolPath;
    QLineEdit* m_txtFFmpegPath;
};
