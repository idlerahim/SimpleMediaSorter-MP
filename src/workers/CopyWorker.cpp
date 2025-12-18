#include "CopyWorker.h"
#include "../core/Utils.h"
#include <QDir>
#include <QDirIterator>
#include <QThread>

CopyWorker::CopyWorker(const Job& job) : Worker(job) {}

void CopyWorker::plan() {
    QString sourcePath = m_job.params["source"].toString();
    QString destPath = m_job.params["dest"].toString();

    if (sourcePath.isEmpty() || destPath.isEmpty()) {
        emit planningFinished(m_job.id, {});
        return;
    }

    emit progress(m_job.id, 0, "Scanning files...");
    QDirIterator it(sourcePath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    
    int count = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        
        JobItem item;
        item.sourcePath = filePath;
        
        QString relativePath = QDir(sourcePath).relativeFilePath(filePath);
        item.destPath = QDir(destPath).filePath(relativePath);
        item.action = "COPY";
        item.status = "PENDING";
        
        m_job.items.push_back(item);
        count++;
        
        if (count % 100 == 0) {
           emit progress(m_job.id, 0, QString("Found %1 files...").arg(count));
        }
    }
    
    emit planningFinished(m_job.id, m_job.items);
}

void CopyWorker::execute() {
    int total = m_job.items.size();
    int current = 0;
    m_success = true;

    for (auto& item : m_job.items) {
        if (QThread::currentThread()->isInterruptionRequested()) break;
        if (item.status != "PENDING") { current++; continue; }

        emit progress(m_job.id, (total > 0 ? (current * 100) / total : 0), QString("Copying %1").arg(QFileInfo(item.sourcePath).fileName()));
        
        QString errorMsg;
        // Ensure dest dir exists
        QDir destDir = QFileInfo(item.destPath).dir();
        if (!destDir.exists()) destDir.mkpath(".");
        
        // Simulate work
        QThread::msleep(50); 

        if (Utils::safeCopy(item.sourcePath, item.destPath, errorMsg)) {
            item.status = "SUCCESS";
            emit itemCompleted(m_job.id, -1, true, item.destPath);
        } else {
            item.status = "ERROR";
            item.reason = errorMsg;
            // m_success = false; // Optional: fail whole job or just item
            emit itemCompleted(m_job.id, -1, false, errorMsg);
        }
        current++;
    }

    emit progress(m_job.id, 100, "Finished");
}
