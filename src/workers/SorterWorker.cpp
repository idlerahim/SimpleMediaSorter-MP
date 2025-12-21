#include "SorterWorker.h"
#include "../core/Utils.h"
#include "../core/ExifToolWrapper.h"
#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QFileInfo>
#include <QThread>
#include <QtConcurrent>
#include <atomic>

SorterWorker::SorterWorker(const Job& job) : Worker(job) {}

void SorterWorker::plan() {
    QString sourcePath = m_job.params["source"].toString();
    QString destPath = m_job.params["dest"].toString();
    QString sortFormat = m_job.params["format"].toString(); // e.g., "yyyy/MM/dd" or "yyyy-MM"

    if (sourcePath.isEmpty() || destPath.isEmpty()) {
        emit planningFinished(m_job.id, {});
        return;
    }
    
    if (sortFormat.isEmpty()) sortFormat = "yyyy/MM/dd";

    emit progress(m_job.id, 0, "Scanning files...");
    QDirIterator it(sourcePath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    
    int count = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        
        JobItem item;
        item.sourcePath = filePath;
        
        QFileInfo fi(filePath);
        // Use file date for now, ideally ExifTool
        QDateTime date = fi.lastModified();
        
        QString relPath = date.toString(sortFormat) + "/" + fi.fileName();
        item.destPath = QDir(destPath).filePath(relPath);
        
        item.action = "MOVE"; // Sorter usually moves
        item.status = "PENDING";
        
        m_job.items.push_back(item);
        count++;
        
         if (count % 50 == 0) {
           emit progress(m_job.id, 0, QString("Found %1 files...").arg(count));
        }
    }
    
    emit planningFinished(m_job.id, m_job.items);
}

void SorterWorker::execute() {
    int total = m_job.items.size();
    if (total == 0) {
        emit progress(m_job.id, 100, "Finished");
        return;
    }

    std::atomic<int> current(0);
    m_success = true;

    // Use a lambda to process each item
    auto processItem = [this, &current, total](JobItem& item) {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        if (item.status != "PENDING") { 
            current++; 
            return; 
        }

        QString errorMsg;
        QDir destDir = QFileInfo(item.destPath).dir();
        if (!destDir.exists()) destDir.mkpath(".");
        
        // Removed artificial delay
        // QThread::msleep(20); 

        // Use rename for move
        if (QFile::rename(item.sourcePath, item.destPath)) {
            item.status = "SUCCESS";
            emit itemCompleted(m_job.id, -1, true, item.destPath);
        } else {
            // If rename failed, try copy + remove for cross-filesystem moves?
            // For now, let's stick to existing logic but maybe add a fallback copy?
            // The original code just failed. I'll stick to original logic but threading it.
            item.status = "ERROR";
            item.reason = "Move failed";
            emit itemCompleted(m_job.id, -1, false, "Move failed: " + item.destPath);
        }
        
        int c = ++current;
        emit progress(m_job.id, (c * 100) / total, QString("Processed %1/%2").arg(c).arg(total));
    };

    QtConcurrent::blockingMap(m_job.items, processItem);
    emit progress(m_job.id, 100, "Finished");
}
