#include "ExifToolWrapper.h"
#include <QDebug>
#include <QFileInfo>

ExifToolWrapper& ExifToolWrapper::instance() {
    static ExifToolWrapper instance;
    return instance;
}

ExifToolWrapper::ExifToolWrapper() {
    // Default path, can be overridden
    m_exifToolPath = "exiftool"; 
}

bool ExifToolWrapper::setExifToolPath(const QString& path) {
    QFileInfo fi(path);
    if (fi.exists() && fi.isExecutable()) {
        m_exifToolPath = path;
        return true;
    }
    return false;
}

QString ExifToolWrapper::getExifToolPath() const {
    return m_exifToolPath;
}

QMap<QString, QString> ExifToolWrapper::readMetadata(const QString& filePath) {
    QMap<QString, QString> metadata;
    
    QProcess process;
    QStringList args;
    args << "-json" << "-n" << filePath;
    
    process.start(m_exifToolPath, args);
    process.waitForFinished();
    
    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        // Parse JSON output
        // For simplicity in this prototype, we'll just return raw output or parse basic tags
        // In a real app, use QJsonDocument
    }
    
    return metadata;
}

bool ExifToolWrapper::writeMetadata(const QString& filePath, const QMap<QString, QString>& tags) {
    if (tags.isEmpty()) return true;

    QProcess process;
    QStringList args;
    args << "-overwrite_original"; // Be careful with this!
    
    for (auto it = tags.begin(); it != tags.end(); ++it) {
        args << QString("-%1=%2").arg(it.key(), it.value());
    }
    args << filePath;

    process.start(m_exifToolPath, args);
    process.waitForFinished();

    return (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0);
}
