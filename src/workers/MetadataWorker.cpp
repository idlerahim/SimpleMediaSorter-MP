#include "MetadataWorker.h"
#include "../core/ExifToolWrapper.h"
#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>

MetadataWorker::MetadataWorker(const Job& job) : Worker(job) {}

void MetadataWorker::plan() {
    emit progress(m_job.id, 0, "Planning rename...");
    
    QString pattern = m_job.params["pattern"].toString();
    QString destDir = m_job.params["dest"].toString();
    
    if (pattern.isEmpty()) pattern = "IMG_{YYYY}{MM}{DD}_{ORIG}";

    int count = 0;
    int total = m_job.items.size();

    // Iterate through items to calculate new paths
    for (auto& item : m_job.items) {
        QFileInfo fi(item.sourcePath);
        if (!fi.exists()) {
            item.status = "ERROR";
            item.reason = "File not found";
            continue;
        }

        // Get date (Simulator: use file modification time, real app uses Exif)
        QDateTime date = fi.lastModified();
        // In real app: date = ExifToolWrapper::instance().getDateTime(item.sourcePath);
        
        QString newName = pattern;
        newName.replace("{YYYY}", date.toString("yyyy"));
        newName.replace("{MM}", date.toString("MM"));
        newName.replace("{DD}", date.toString("dd"));
        newName.replace("{ORIG}", fi.baseName());
        
        // Add extension
        newName += "." + fi.suffix();

        if (!destDir.isEmpty()) {
            QDir d(destDir);
            item.destPath = d.filePath(newName);
        } else {
            item.destPath = fi.dir().filePath(newName);
        }

        item.action = "RENAME";
        item.status = "PENDING";
        
        count++;
        emit progress(m_job.id, (count * 100) / total, "Analyzed " + fi.fileName());
    }

    emit planningFinished(m_job.id, m_job.items);
}

void MetadataWorker::execute() {
    int count = 0;
    int total = m_job.items.size();
    m_success = true;

    for (auto& item : m_job.items) {
        if (item.status != "PENDING") {
            count++;
            continue;
        }
        
        emit progress(m_job.id, (count * 100) / total, "Renaming " + QFileInfo(item.sourcePath).fileName());
        
        QDir dir = QFileInfo(item.destPath).dir();
        if (!dir.exists()) dir.mkpath(".");

        // Check collision
        if (QFile::exists(item.destPath)) {
             // Simple skip policy for now
             item.status = "SKIPPED";
             item.reason = "Destination exists";
             emit itemCompleted(m_job.id, -1, false, "Skipped: " + item.destPath);
        } else {
            if (QFile::rename(item.sourcePath, item.destPath)) {
                item.status = "SUCCESS";
                emit itemCompleted(m_job.id, -1, true, "Renamed to " + item.destPath);
            } else {
                item.status = "ERROR";
                item.reason = "Rename failed";
                m_success = false;
                emit itemCompleted(m_job.id, -1, false, "Failed to rename: " + item.sourcePath);
            }
        }
        count++;
    }
    
    emit progress(m_job.id, 100, "Done");
}
