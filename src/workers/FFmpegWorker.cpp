#include "FFmpegWorker.h"
#include "../core/FFmpegWrapper.h"
#include <QThread>
#include <QtConcurrent>
#include <atomic>

FFmpegWorker::FFmpegWorker(const Job& job) : Worker(job) {}

#include <QDir>
#include <QDirIterator>

void FFmpegWorker::plan() {
    QString sourcePath = m_job.params["source"].toString();
    QString destPath = m_job.params["dest"].toString();
    QString container = m_job.params["container"].toString();
    if (container.isEmpty()) container = "mp4";

    if (sourcePath.isEmpty() || destPath.isEmpty()) {
        emit planningFinished(m_job.id, {});
        return;
    }

    emit progress(m_job.id, 0, "Scanning videos...");
    QStringList extensions = {"*.mp4", "*.mkv", "*.avi", "*.mov", "*.flv", "*.webm"};
    QDirIterator it(sourcePath, extensions, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    
    int count = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        JobItem item;
        item.sourcePath = filePath;
        
        QFileInfo fi(filePath);
        QString relPath = QDir(sourcePath).relativeFilePath(filePath);
        // Change extension
        QString newRelPath = QFileInfo(relPath).path() + "/" + fi.completeBaseName() + "." + container;
        if (newRelPath.startsWith("./")) newRelPath = newRelPath.mid(2);

        item.destPath = QDir(destPath).filePath(newRelPath);
        item.action = "TRANSCODE";
        item.status = "PENDING";
        
        m_job.items.push_back(item);
        count++;
        
         if (count % 20 == 0) {
           emit progress(m_job.id, 0, QString("Found %1 videos...").arg(count));
        }
    }
    emit planningFinished(m_job.id, m_job.items);
}

void FFmpegWorker::execute() {
    int total = m_job.items.size();
    if (total == 0) {
        emit progress(m_job.id, 100, "Finished");
        return;
    }

    std::atomic<int> current(0);
    m_success = true;
    
    QString codec = m_job.params["codec"].toString();
    if (codec.isEmpty()) codec = "copy"; // Default fast copy

    auto processItem = [this, &current, total, codec](JobItem& item) {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        if (item.status != "PENDING") { 
            current++;
            return;
        }

        QString progressStr = QString("Processing %1").arg(QFileInfo(item.sourcePath).fileName());
        // We can't easily emit progress per item update without flooding, so we just log per item start/finish or periodic?
        // Let's rely on atomic current for overall progress.
        
        QDir destDir = QFileInfo(item.destPath).dir();
        if (!destDir.exists()) destDir.mkpath(".");

        QStringList args;
        args << "-y" << "-i" << item.sourcePath << "-c:v" << codec << item.destPath;
        
        // Use Wrapper
        bool res = FFmpegWrapper::instance().runCommand(args, [this](int pct, QString msg){
             // Parse FFmpeg output for progress if possible
             // We could emit detailed sub-progress if supported, but tricky with multiple threads.
             // Keep it simple.
        });
        
        // Removed simulation delay
        // if (!res) { QThread::msleep(500); }

        if (res) {
            item.status = "SUCCESS";
            emit itemCompleted(m_job.id, -1, true, item.destPath);
        } else {
             // If failed (maybe no ffmpeg), mark error
             // But if we want to "mock" success for validation if ffmpeg missing?
             // User wants "fast", removing mock delays is key.
             // If ffmpeg is missing, it will fail fast. That is correct.
             item.status = "ERROR";
             item.reason = "FFmpeg failed";
             emit itemCompleted(m_job.id, -1, false, "FFmpeg failed (check logs)");
        }
        
        int c = ++current;
        emit progress(m_job.id, (c * 100) / total, QString("Processed %1/%2").arg(c).arg(total));
    };

    QtConcurrent::blockingMap(m_job.items, processItem);
    emit progress(m_job.id, 100, "Finished");
}
