#pragma once

#include "../components/JobBaseWidget.h"
#include <QLineEdit>
#include <QCheckBox>

class BackupWidget : public JobBaseWidget {
    Q_OBJECT
public:
    explicit BackupWidget(QWidget *parent = nullptr);
    void init(); // Optional if we use init pattern
protected:
    QWidget* createConfigWidget() override;
    Worker* createWorker() override;
    QString getTitle() const override { return "Backup / Copy"; }

private:
    QLineEdit* m_srcEdit;
    QLineEdit* m_destEdit;
};
