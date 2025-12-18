#pragma once

#include "../components/JobBaseWidget.h"
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

class SorterWidget : public JobBaseWidget {
    Q_OBJECT
public:
    explicit SorterWidget(QWidget *parent = nullptr);
    void init();
protected:
    QWidget* createConfigWidget() override;
    Worker* createWorker() override;
    QString getTitle() const override { return "Media Sorter"; }

private:
    QLineEdit* m_srcEdit;
    QLineEdit* m_destEdit;
    QComboBox* m_formatCombo;
};
