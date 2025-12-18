#pragma once

#include "../components/JobBaseWidget.h"
#include <QLineEdit>
#include <QComboBox>

class VideoWidget : public JobBaseWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    void init();
protected:
    QWidget* createConfigWidget() override;
    Worker* createWorker() override;
    QString getTitle() const override { return "FFMPEG Video Transcoder"; }

private:
    QLineEdit* m_srcEdit;
    QLineEdit* m_destEdit;
    QComboBox* m_codecCombo;
    QComboBox* m_containerCombo;
};
