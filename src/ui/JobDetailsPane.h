#pragma once

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include "../core/Job.h"

class JobDetailsPane : public QWidget {
    Q_OBJECT
public:
    explicit JobDetailsPane(QWidget *parent = nullptr);

public slots:
    void setJob(const Job& job);
    void clear();

private:
    QLabel* m_lblId;
    QLabel* m_lblType;
    QLabel* m_lblState;
    QLabel* m_lblProgress;
    QTextEdit* m_txtParams;
    QTextEdit* m_txtLogs;
};
