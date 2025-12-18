#pragma once

#include <QString>
#include <QFileInfo>

namespace Utils {
    // Generate a unique filename if destination exists (e.g., file.txt -> file_1.txt)
    QString getUniqueFilePath(const QString& filePath);
    
    // Safe copy with overwrite protection
    bool safeCopy(const QString& source, const QString& dest, QString& errorMsg);
    
    // Recursive directory size
    qint64 dirSize(const QString& path);
}
