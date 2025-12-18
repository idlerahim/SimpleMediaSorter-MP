#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <vector>

enum class JobState {
    Pending,
    Running,
    Paused,
    Finished,
    Failed,
    Cancelled
};

struct JobItem {
    int id = -1;
    int jobId = -1;
    QString sourcePath;
    QString destPath;
    QString action; // "COPY", "MOVE", "RENAME", "DELETE"
    QString status; // "PENDING", "SUCCESS", "ERROR", "SKIPPED"
    QString reason;
    int retries = 0;
};

struct Job {
    int id = -1;
    QString type;
    QJsonObject params;
    JobState state = JobState::Pending;
    QDateTime createdAt;
    QDateTime startedAt;
    QDateTime finishedAt;
    
    // Runtime only, not persisted directly in 'jobs' table (items are separate)
    std::vector<JobItem> items; 
    int progress = 0;
    QString statusMessage;
};
