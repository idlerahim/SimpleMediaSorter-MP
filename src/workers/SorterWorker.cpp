#include "SorterWorker.h"
#include "../core/Utils.h"
#include "../core/ExifToolWrapper.h"
#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QFileInfo>
#include <QThread>

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
    int current = 0;
    m_success = true;

    for (auto& item : m_job.items) {
        if (QThread::currentThread()->isInterruptionRequested()) break;
        if (item.status != "PENDING") { current++; continue; }

        emit progress(m_job.id, (total > 0 ? (current * 100) / total : 0), QString("Sorting %1").arg(QFileInfo(item.sourcePath).fileName()));
        
        QString errorMsg;
        QDir destDir = QFileInfo(item.destPath).dir();
        if (!destDir.exists()) destDir.mkpath(".");
        
        QThread::msleep(20); 

        // Use rename for move
        if (QFile::rename(item.sourcePath, item.destPath)) {
            item.status = "SUCCESS";
            emit itemCompleted(m_job.id, -1, true, item.destPath);
        } else {
            // Try copy then delete if rename fails (cross-filesystem) ? 
            // For now simple rename.
            item.status = "ERROR";
            item.reason = "Move failed";
            emit itemCompleted(m_job.id, -1, false, "Move failed: " + item.destPath);
        }
        current++;
    }

    emit progress(m_job.id, 100, "Finished");
}
