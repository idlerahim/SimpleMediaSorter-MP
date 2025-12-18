#include "JobModel.h"
#include "DatabaseManager.h"
#include "JobManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QDebug>

JobModel::JobModel(QObject *parent) : QAbstractTableModel(parent) {
    loadJobs();

    connect(&JobManager::instance(), &JobManager::jobAdded, this, &JobModel::onJobAdded);
    connect(&JobManager::instance(), &JobManager::jobStarted, this, &JobModel::onJobStarted);
    connect(&JobManager::instance(), &JobManager::jobProgress, this, &JobModel::onJobProgress);
    connect(&JobManager::instance(), &JobManager::jobFinished, this, &JobModel::onJobFinished);
}

int JobModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_jobs.size());
}

int JobModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return 5; // ID, Type, State, Progress, Status
}

QVariant JobModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_jobs.size()) return QVariant();

    const Job& job = m_jobs[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return job.id;
            case 1: return job.type;
            case 2: {
                switch(job.state) {
                    case JobState::Pending: return "Pending";
                    case JobState::Running: return "Running";
                    case JobState::Finished: return "Finished";
                    case JobState::Failed: return "Failed";
                    default: return "Unknown";
                }
            }
            case 3: return QString("%1%").arg(job.progress);
            case 4: return job.statusMessage;
        }
    }
    
    return QVariant();
}

QVariant JobModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section) {
        case 0: return "ID";
        case 1: return "Type";
        case 2: return "State";
        case 3: return "Progress";
        case 4: return "Status";
        default: return QVariant();
    }
}

void JobModel::reload() {
    beginResetModel();
    loadJobs();
    endResetModel();
}

const Job& JobModel::getJob(int row) const {
    static Job emptyJob;
    if (row >= 0 && row < m_jobs.size()) {
        return m_jobs[row];
    }
    return emptyJob;
}

void JobModel::loadJobs() {
    m_jobs.clear();
    QSqlQuery query("SELECT * FROM jobs ORDER BY id DESC");
    while (query.next()) {
        Job job;
        job.id = query.value("id").toInt();
        job.type = query.value("type").toString();
        // job.params = ...; // Parse if needed
        QString stateStr = query.value("state").toString();
        if (stateStr == "PENDING") job.state = JobState::Pending;
        else if (stateStr == "RUNNING") job.state = JobState::Running;
        else if (stateStr == "FINISHED") job.state = JobState::Finished;
        else if (stateStr == "FAILED") job.state = JobState::Failed;
        
        m_jobs.push_back(job);
    }
}

int JobModel::findJobIndex(int jobId) const {
    for (size_t i = 0; i < m_jobs.size(); ++i) {
        if (m_jobs[i].id == jobId) return static_cast<int>(i);
    }
    return -1;
}

void JobModel::onJobAdded(int jobId) {
    reload(); // Simplest way for now, optimize later to insertRow
}

void JobModel::onJobStarted(int jobId) {
    int idx = findJobIndex(jobId);
    if (idx != -1) {
        m_jobs[idx].state = JobState::Running;
        emit dataChanged(index(idx, 2), index(idx, 2));
    }
}

void JobModel::onJobProgress(int jobId, int progress, const QString& message) {
    int idx = findJobIndex(jobId);
    if (idx != -1) {
        m_jobs[idx].progress = progress;
        m_jobs[idx].statusMessage = message;
        emit dataChanged(index(idx, 3), index(idx, 4));
    }
}

void JobModel::onJobFinished(int jobId, bool success) {
    int idx = findJobIndex(jobId);
    if (idx != -1) {
        m_jobs[idx].state = success ? JobState::Finished : JobState::Failed;
        m_jobs[idx].progress = success ? 100 : 0;
        emit dataChanged(index(idx, 2), index(idx, 4));
    }
}
