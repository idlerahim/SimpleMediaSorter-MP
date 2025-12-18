#include "JobListView.h"
#include "JobDetailsPane.h"
#include "../core/JobManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QJsonObject>

JobListView::JobListView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_btnNewJob = new QPushButton("New Job (Test)", this);
    mainLayout->addWidget(m_btnNewJob);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // Left: Table View
    m_tableView = new QTableView(this);
    m_model = new JobModel(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    splitter->addWidget(m_tableView);

    // Right: Details Pane
    JobDetailsPane* detailsPane = new JobDetailsPane(this);
    splitter->addWidget(detailsPane);
    
    // Set initial sizes
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    connect(m_btnNewJob, &QPushButton::clicked, this, &JobListView::onNewJobClicked);
    
    // Connect selection change to details pane
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, 
        [this, detailsPane](const QModelIndex &current, const QModelIndex &previous) {
            if (current.isValid()) {
                const Job& job = m_model->getJob(current.row());
                detailsPane->setJob(job);
            } else {
                detailsPane->clear();
            }
    });
}

void JobListView::onNewJobClicked() {
    // For testing purposes, add a dummy job
    QJsonObject params;
    params["source"] = "C:/Test/Source";
    params["dest"] = "C:/Test/Dest";
    JobManager::instance().addJob("TEST_COPY", params);
}
