#pragma once

#include "../components/JobBaseWidget.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

class ImageWidget : public JobBaseWidget {
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = nullptr);
    void init();
protected:
    QWidget* createConfigWidget() override;
    Worker* createWorker() override;
    QString getTitle() const override { return "Image Processor"; }

private:
    QLineEdit* m_srcEdit;
    QLineEdit* m_destEdit;
    QSpinBox* m_widthSpin;
    QSpinBox* m_qualitySpin;
    QComboBox* m_formatCombo;
};
