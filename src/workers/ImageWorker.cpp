#include "ImageWorker.h"
#include <QDir>
#include <QDirIterator>
#include <QImage>
#include <QThread>
#include <QtConcurrent>
#include <atomic>

ImageWorker::ImageWorker(const Job& job) : Worker(job) {}

void ImageWorker::plan() {
    QString sourcePath = m_job.params["source"].toString();
    QString destPath = m_job.params["dest"].toString();
    QString format = m_job.params["format"].toString();
    if (format.isEmpty()) format = "jpg";

    if (sourcePath.isEmpty() || destPath.isEmpty()) {
        emit planningFinished(m_job.id, {});
        return;
    }

    emit progress(m_job.id, 0, "Scanning images...");
    QStringList extensions = {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.webp"};
    QDirIterator it(sourcePath, extensions, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    
    int count = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        JobItem item;
        item.sourcePath = filePath;
        
        QFileInfo fi(filePath);
        QString relPath = QDir(sourcePath).relativeFilePath(filePath);
        // Change extension
        QString newRelPath = QFileInfo(relPath).path() + "/" + fi.completeBaseName() + "." + format;
        if (newRelPath.startsWith("./")) newRelPath = newRelPath.mid(2); // Cleanup

        item.destPath = QDir(destPath).filePath(newRelPath);
        item.action = "CONVERT";
        item.status = "PENDING";
        
        m_job.items.push_back(item);
        count++;

        if (count % 50 == 0) {
           emit progress(m_job.id, 0, QString("Found %1 images...").arg(count));
        }
    }
    emit planningFinished(m_job.id, m_job.items);
}

void ImageWorker::execute() {
    int total = m_job.items.size();
    if (total == 0) {
        emit progress(m_job.id, 100, "Finished");
        return;
    }

    std::atomic<int> current(0);
    m_success = true;
    
    int width = m_job.params["width"].toInt();
    int quality = m_job.params["quality"].toInt();
    QString format = m_job.params["format"].toString();
    if (quality <= 0) quality = 90;

    // Use a lambda to process each item
    auto processItem = [this, &current, total, width, quality, format](JobItem& item) {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        if (item.status != "PENDING") { 
            current++;
            return; 
        }

        // Parallel processing of images
        QDir destDir = QFileInfo(item.destPath).dir();
        if (!destDir.exists()) destDir.mkpath(".");

        QImage img;
        if (img.load(item.sourcePath)) {
            if (width > 0) {
                 img = img.scaledToWidth(width, Qt::SmoothTransformation);
            }
            
            if (img.save(item.destPath, format.toLatin1().constData(), quality)) {
                item.status = "SUCCESS";
                emit itemCompleted(m_job.id, -1, true, item.destPath);
            } else {
                 item.status = "ERROR";
                 item.reason = "Save failed";
                 emit itemCompleted(m_job.id, -1, false, "Save failed: " + item.destPath);
            }
        } else {
            item.status = "ERROR";
            item.reason = "Load failed";
            emit itemCompleted(m_job.id, -1, false, "Load failed: " + item.sourcePath);
        }
        
        int c = ++current;
        emit progress(m_job.id, (c * 100) / total, QString("Processed %1/%2").arg(c).arg(total));
    };

    QtConcurrent::blockingMap(m_job.items, processItem);
    emit progress(m_job.id, 100, "Finished");
}
