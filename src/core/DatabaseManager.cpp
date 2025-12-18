#include "DatabaseManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

std::unique_ptr<DatabaseManager> DatabaseManager::m_instance;
QMutex DatabaseManager::m_mutex;

DatabaseManager& DatabaseManager::instance() {
    QMutexLocker locker(&m_mutex);
    if (!m_instance) {
        m_instance.reset(new DatabaseManager());
    }
    return *m_instance;
}

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::init() {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString dbPath = dir.filePath("smsorter.db");
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "Error: connection with database failed" << m_db.lastError();
        return false;
    } else {
        qDebug() << "Database: connection ok" << dbPath;
    }

    QSqlQuery query;
    
    // Jobs table
    if (!query.exec("CREATE TABLE IF NOT EXISTS jobs ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "type TEXT, "
                    "params TEXT, "
                    "state TEXT, "
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
                    "started_at DATETIME, "
                    "finished_at DATETIME)")) {
        qCritical() << "Error creating jobs table:" << query.lastError();
        return false;
    }

    // Job Items table
    if (!query.exec("CREATE TABLE IF NOT EXISTS job_items ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "job_id INTEGER, "
                    "source_path TEXT, "
                    "dest_path TEXT, "
                    "status TEXT, "
                    "retries INTEGER DEFAULT 0, "
                    "reason TEXT, "
                    "FOREIGN KEY(job_id) REFERENCES jobs(id))")) {
        qCritical() << "Error creating job_items table:" << query.lastError();
        return false;
    }

    return true;
}

QSqlDatabase DatabaseManager::database() const {
    return m_db;
}
