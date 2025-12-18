#include "JobManager.h"
#include "DatabaseManager.h"
#include "WorkerFactory.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QDebug>

JobManager& JobManager::instance() {
    static JobManager instance;
    return instance;
}

JobManager::JobManager() : m_isPaused(false) {
    m_threadPool.setMaxThreadCount(4);
}

JobManager::~JobManager() {
    m_threadPool.waitForDone();
}

void JobManager::addJob(const QString& type, const QJsonObject& params) {
    QSqlQuery query;
    query.prepare("INSERT INTO jobs (type, params, state) VALUES (:type, :params, 'PENDING')");
    query.bindValue(":type", type);
    query.bindValue(":params", QJsonDocument(params).toJson(QJsonDocument::Compact));
    
    if (query.exec()) {
        int jobId = query.lastInsertId().toInt();
        
        Job job;
        job.id = jobId;
        job.type = type;
        job.params = params;
        job.state = JobState::Pending;

        emit jobAdded(jobId);
        qDebug() << "Job added:" << jobId;
        
        processQueue();
    } else {
        qCritical() << "Failed to add job:" << query.lastError();
    }
}

void JobManager::processQueue() {
    if (m_isPaused) return;

    // Simple logic: find pending jobs and start them
    // In a real app, we'd query the DB for PENDING jobs
    // For now, we rely on the fact that addJob triggers this.
    // We need to fetch the job details from DB to be sure.
    
    QSqlQuery query("SELECT id, type, params FROM jobs WHERE state = 'PENDING' LIMIT 1");
    if (query.next()) {
        Job job;
        job.id = query.value("id").toInt();
        job.type = query.value("type").toString();
        job.params = QJsonDocument::fromJson(query.value("params").toByteArray()).object();
        
        startJob(job);
    }
}

void JobManager::startJob(Job& job) {
    Worker* worker = WorkerFactory::createWorker(job);
    if (worker) {
        // Update DB state
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE jobs SET state = 'RUNNING', started_at = CURRENT_TIMESTAMP WHERE id = :id");
        updateQuery.bindValue(":id", job.id);
        updateQuery.exec();

        connect(worker, &Worker::started, this, &JobManager::jobStarted);
        connect(worker, &Worker::progress, this, &JobManager::jobProgress);
        connect(worker, &Worker::finished, this, &JobManager::onJobFinished);
        
        m_threadPool.start(worker);
    } else {
        qWarning() << "No worker found for type:" << job.type;
    }
}

void JobManager::onJobFinished(int jobId, bool success) {
    QSqlQuery query;
    query.prepare("UPDATE jobs SET state = :state, finished_at = CURRENT_TIMESTAMP WHERE id = :id");
    query.bindValue(":state", success ? "FINISHED" : "FAILED");
    query.bindValue(":id", jobId);
    query.exec();

    emit jobFinished(jobId, success);
    processQueue(); // Pick up next job
}

void JobManager::startQueue() {
    m_isPaused = false;
    processQueue();
}

void JobManager::pauseQueue() {
    m_isPaused = true;
}
