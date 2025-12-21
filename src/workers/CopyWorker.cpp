#include "CopyWorker.h"
#include "../core/Utils.h"
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent>
#include <atomic>

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
    if (total == 0) {
        emit progress(m_job.id, 100, "Finished");
        return;
    }

    std::atomic<int> current(0);
    // m_success is tracked per item, but we can track overall success if needed.
    // For now, let's say overall is success if no fatal errors occurred? 
    // Actually, Worker.h has m_success member.
    m_success = true;

    // Use a lambda to process each item
    auto processItem = [this, &current, total](JobItem& item) {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        if (item.status != "PENDING") { 
            current++; 
            return; 
        }

        // Emit progress periodically or for every item?
        // Emitting signals from many threads rapidly can be overwhelming.
        // Let's emit only on completion or periodically.
        // Since we are inside a blockingMap, we are in a thread pool thread.
        
        QString errorMsg;
        // Ensure dest dir exists
        // Note: multiple threads creating same dir might be racey, but mkpath is generally safe or fails gracefully.
        QDir destDir = QFileInfo(item.destPath).dir();
        if (!destDir.exists()) destDir.mkpath(".");

        // Removed artificial delay
        // QThread::msleep(50); 

        if (Utils::safeCopy(item.sourcePath, item.destPath, errorMsg)) {
            item.status = "SUCCESS";
            // emit itemCompleted is thread-safe (queued connection)
            emit itemCompleted(m_job.id, -1, true, item.destPath);
        } else {
            item.status = "ERROR";
            item.reason = errorMsg;
            emit itemCompleted(m_job.id, -1, false, errorMsg);
        }
        
        int c = ++current;
        emit progress(m_job.id, (c * 100) / total, QString("Processed %1/%2").arg(c).arg(total));
    };

    // Use QtConcurrent::blockingMap to process in parallel and wait for finish
    QtConcurrent::blockingMap(m_job.items, processItem);

    emit progress(m_job.id, 100, "Finished");
}
