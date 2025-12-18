#pragma once

#include <QString>
#include <QProcess>
#include <functional>

class FFmpegWrapper {
public:
    static FFmpegWrapper& instance();

    bool setFFmpegPath(const QString& path);
    QString getFFmpegPath() const;

    // Run ffmpeg command
    // progressCallback: (percent, message)
    bool runCommand(const QStringList& args, std::function<void(int, QString)> progressCallback);

    // Get media duration in seconds
    double getDuration(const QString& filePath);

private:
    FFmpegWrapper();
    QString m_ffmpegPath;
    QString m_ffprobePath;
};
