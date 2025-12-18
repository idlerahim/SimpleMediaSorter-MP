#include "Utils.h"
#include <QFile>
#include <QDir>
#include <QDirIterator>

QString Utils::getUniqueFilePath(const QString& filePath) {
    QFileInfo fi(filePath);
    if (!fi.exists()) return filePath;

    QString path = fi.path();
    QString baseName = fi.completeBaseName();
    QString suffix = fi.suffix();
    if (!suffix.isEmpty()) suffix.prepend(".");

    int counter = 1;
    QString newPath;
    do {
        newPath = QString("%1/%2_%3%4").arg(path, baseName).arg(counter).arg(suffix);
        counter++;
    } while (QFile::exists(newPath));

    return newPath;
}

bool Utils::safeCopy(const QString& source, const QString& dest, QString& errorMsg) {
    QFile srcFile(source);
    if (!srcFile.exists()) {
        errorMsg = "Source file does not exist";
        return false;
    }

    QString finalDest = dest;
    if (QFile::exists(finalDest)) {
        finalDest = getUniqueFilePath(finalDest);
    }

    // Create parent dir if needed
    QDir dir = QFileInfo(finalDest).dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if (!srcFile.copy(finalDest)) {
        errorMsg = srcFile.errorString();
        return false;
    }

    return true;
}

qint64 Utils::dirSize(const QString& path) {
    qint64 size = 0;
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        size += it.fileInfo().size();
    }
    return size;
}
