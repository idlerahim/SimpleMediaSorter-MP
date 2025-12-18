#pragma once

#include <QSqlDatabase>
#include <QMutex>
#include <memory>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool init();
    QSqlDatabase database() const;

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
    static std::unique_ptr<DatabaseManager> m_instance;
    static QMutex m_mutex;

    friend std::unique_ptr<DatabaseManager>;
    friend std::default_delete<DatabaseManager>;
};
