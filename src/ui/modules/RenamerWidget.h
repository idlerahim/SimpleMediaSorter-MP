#pragma once

#include "../components/JobBaseWidget.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QListWidget>

class RenamerWidget : public JobBaseWidget {
    Q_OBJECT
public:
    explicit RenamerWidget(QWidget *parent = nullptr);

protected:
    QWidget* createConfigWidget() override;
    Worker* createWorker() override;
    QString getTitle() const override { return "Renamer"; }

private:
    void init(); // Helper to run logic that calls createConfigWidget
    
    QListWidget* m_filesList;
    QLineEdit* m_patternEdit;
    QLineEdit* m_destEdit;
    QCheckBox* m_regexCheck;
};
