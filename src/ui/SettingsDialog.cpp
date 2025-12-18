#include "SettingsDialog.h"
#include "../core/ExifToolWrapper.h"
#include "../core/FFmpegWrapper.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Settings");
    resize(400, 200);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    m_txtExifToolPath = new QLineEdit(ExifToolWrapper::instance().getExifToolPath());
    m_txtFFmpegPath = new QLineEdit(FFmpegWrapper::instance().getFFmpegPath());

    formLayout->addRow("ExifTool Path:", m_txtExifToolPath);
    formLayout->addRow("FFmpeg Path:", m_txtFFmpegPath);

    mainLayout->addLayout(formLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveSettings);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void SettingsDialog::saveSettings() {
    ExifToolWrapper::instance().setExifToolPath(m_txtExifToolPath->text());
    FFmpegWrapper::instance().setFFmpegPath(m_txtFFmpegPath->text());
    accept();
}
