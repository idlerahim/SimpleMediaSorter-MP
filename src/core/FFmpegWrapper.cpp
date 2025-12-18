#include "FFmpegWrapper.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>

FFmpegWrapper& FFmpegWrapper::instance() {
    static FFmpegWrapper instance;
    return instance;
}

FFmpegWrapper::FFmpegWrapper() {
    m_ffmpegPath = "ffmpeg";
    m_ffprobePath = "ffprobe";
}

bool FFmpegWrapper::setFFmpegPath(const QString& path) {
    QFileInfo fi(path);
    if (fi.exists() && fi.isExecutable()) {
        m_ffmpegPath = path;
        // Assume ffprobe is next to it
        QFileInfo probeFi(fi.dir().filePath("ffprobe.exe"));
        if (probeFi.exists()) {
            m_ffprobePath = probeFi.absoluteFilePath();
        }
        return true;
    }
    return false;
}

QString FFmpegWrapper::getFFmpegPath() const {
    return m_ffmpegPath;
}

double FFmpegWrapper::getDuration(const QString& filePath) {
    QProcess process;
    QStringList args;
    args << "-v" << "error" << "-show_entries" << "format=duration" << "-of" << "default=noprint_wrappers=1:nokey=1" << filePath;
    
    process.start(m_ffprobePath, args);
    process.waitForFinished();
    
    QString output = process.readAllStandardOutput();
    return output.toDouble();
}

bool FFmpegWrapper::runCommand(const QStringList& args, std::function<void(int, QString)> progressCallback) {
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    
    process.start(m_ffmpegPath, args);
    
    if (!process.waitForStarted()) return false;

    // Parse duration from output if possible to calculate progress
    // For now, just stream output
    
    while (process.state() == QProcess::Running) {
        if (process.waitForReadyRead(100)) {
            QString output = process.readAll();
            // Simple parsing logic would go here
            // "time=00:00:05.20"
            
            if (progressCallback) {
                progressCallback(-1, output.trimmed());
            }
        }
    }

    return (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0);
}
