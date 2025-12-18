#include "ImageWorker.h"
#include <QDir>
#include <QDirIterator>
#include <QImage>
#include <QThread>

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
    int current = 0;
    m_success = true;
    
    int width = m_job.params["width"].toInt();
    int quality = m_job.params["quality"].toInt();
    QString format = m_job.params["format"].toString();
    if (quality <= 0) quality = 90;

    for (auto& item : m_job.items) {
        if (QThread::currentThread()->isInterruptionRequested()) break;
        if (item.status != "PENDING") { current++; continue; }

        emit progress(m_job.id, (total > 0 ? (current * 100) / total : 0), QString("Processing %1").arg(QFileInfo(item.sourcePath).fileName()));
        
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
        current++;
    }
    emit progress(m_job.id, 100, "Finished");
}
