#pragma once

#include <QRunnable>
#include <QObject>
#include "Job.h"

class Worker : public QObject, public QRunnable {
    Q_OBJECT
public:
    Worker(const Job& job) : m_job(job) {
        setAutoDelete(true);
    }
    virtual ~Worker() = default;

    void run() override;

    enum class Mode {
        Planning,
        Execution
    };
    
    void setMode(Mode mode) { m_mode = mode; }
    Mode mode() const { return m_mode; }
    void setItems(const std::vector<JobItem>& items) { m_job.items = items; }

    // Worker implementations must implement these
    virtual void plan() = 0;
    virtual void execute() = 0;

signals:
    void started(int jobId);
    void planningFinished(int jobId, const std::vector<JobItem>& items);
    void progress(int jobId, int percent, const QString& message);
    void finished(int jobId, bool success);
    void itemCompleted(int jobId, int itemId, bool success, const QString& message);

protected:
    Job m_job;
    bool m_success = false;
    Mode m_mode = Mode::Execution;
};
