#pragma once

#include <QString>
#include <QMap>
#include <QProcess>

class ExifToolWrapper {
public:
    static ExifToolWrapper& instance();

    bool setExifToolPath(const QString& path);
    QString getExifToolPath() const;

    // Read metadata
    QMap<QString, QString> readMetadata(const QString& filePath);

    // Write metadata
    bool writeMetadata(const QString& filePath, const QMap<QString, QString>& tags);

private:
    ExifToolWrapper();
    QString m_exifToolPath;
};
