#pragma once

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include "../core/JobModel.h"

class JobListView : public QWidget {
    Q_OBJECT
public:
    explicit JobListView(QWidget *parent = nullptr);

private slots:
    void onNewJobClicked();

private:
    QTableView* m_tableView;
    JobModel* m_model;
    QPushButton* m_btnNewJob;

public:
    QTableView* view() const { return m_tableView; }
    JobModel* model() const { return m_model; }
};
