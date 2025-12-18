#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "Job.h"

class JobModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit JobModel(QObject *parent = nullptr);

    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        StateRole,
        ProgressRole,
        StatusMessageRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void reload();
    const Job& getJob(int row) const;

public slots:
    void onJobAdded(int jobId);
    void onJobStarted(int jobId);
    void onJobProgress(int jobId, int progress, const QString& message);
    void onJobFinished(int jobId, bool success);

private:
    std::vector<Job> m_jobs;
    
    void loadJobs();
    int findJobIndex(int jobId) const;
};
