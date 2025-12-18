#pragma once

#include <QObject>
#include <QThreadPool>
#include <QQueue>
#include <QMutex>

// Forward declarations
struct Job; 

class JobManager : public QObject {
    Q_OBJECT
public:
    static JobManager& instance();

    void addJob(const QString& type, const QJsonObject& params);
    void startQueue();
    void pauseQueue();

signals:
    void jobAdded(int jobId);
    void jobStarted(int jobId);
    void jobProgress(int jobId, int progress, const QString& message);
    void jobFinished(int jobId, bool success);

private slots:
    void onJobFinished(int jobId, bool success);

private:
    JobManager();
    ~JobManager();
    JobManager(const JobManager&) = delete;
    JobManager& operator=(const JobManager&) = delete;

    void processQueue();
    void startJob(Job& job);

    QThreadPool m_threadPool;
    bool m_isPaused;
};
