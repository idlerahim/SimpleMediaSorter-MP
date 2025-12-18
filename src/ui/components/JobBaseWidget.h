#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include "../../core/Worker.h"

class JobBaseWidget : public QWidget {
    Q_OBJECT
public:
    explicit JobBaseWidget(QWidget *parent = nullptr);
    virtual ~JobBaseWidget() = default;

protected:
    // Pure virtuals for subclasses to implement
    virtual QWidget* createConfigWidget() = 0;
    virtual Worker* createWorker() = 0; 
    virtual QString getTitle() const = 0;

    // Helper to get selected files (to be implemented by subclasses or passed via generic mechanism)
    // For now we assume the Config widget interacts with the specific Worker params

    void log(const QString& message);

protected slots:
    void onPlanClicked();
    void onRunClicked();
    void onBackClicked();
    void onWorkerPlanningFinished(int jobId, const std::vector<JobItem>& items);
    void onWorkerFinished(int jobId, bool success);
    void onWorkerProgress(int jobId, int percent, const QString& msg);

protected:
    void setupUI();
    void buildPreviewTable(const std::vector<JobItem>& items);

    QStackedWidget* m_stack;
    QWidget* m_configPage;
    QWidget* m_previewPage;
    QWidget* m_executionPage;

    // Preview elements
    QTableWidget* m_previewTable;
    QPushButton* m_btnRun;
    
    // Execution elements
    QProgressBar* m_progressBar;
    QTextEdit* m_logView;
    QPushButton* m_btnDone;

    // Current state
    Worker* m_worker = nullptr;
    std::vector<JobItem> m_plannedItems;
};
