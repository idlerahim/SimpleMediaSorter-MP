#include "JobBaseWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QThreadPool>

JobBaseWidget::JobBaseWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void JobBaseWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    m_stack = new QStackedWidget(this);
    mainLayout->addWidget(m_stack);

    // -- Stage 1: Config (Placeholder, actual content set in derived classes) --
    m_configPage = new QWidget();
    // Derived classes will populate this, we just need a container
    m_stack->addWidget(m_configPage);

    // -- Stage 2: Preview --
    m_previewPage = new QWidget();
    QVBoxLayout* previewLayout = new QVBoxLayout(m_previewPage);
    
    QLabel* prevTitle = new QLabel("<h3>Plan & Preview</h3>", m_previewPage);
    previewLayout->addWidget(prevTitle);

    m_previewTable = new QTableWidget(m_previewPage);
    m_previewTable->setColumnCount(3);
    m_previewTable->setHorizontalHeaderLabels({"Source", "Destination", "Action"});
    m_previewTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    previewLayout->addWidget(m_previewTable);

    QHBoxLayout* prevBtnLayout = new QHBoxLayout();
    QPushButton* btnBack = new QPushButton("Back", m_previewPage);
    m_btnRun = new QPushButton("Execute", m_previewPage);
    connect(btnBack, &QPushButton::clicked, this, &JobBaseWidget::onBackClicked);
    connect(m_btnRun, &QPushButton::clicked, this, &JobBaseWidget::onRunClicked);
    
    prevBtnLayout->addStretch();
    prevBtnLayout->addWidget(btnBack);
    prevBtnLayout->addWidget(m_btnRun);
    previewLayout->addLayout(prevBtnLayout);

    m_stack->addWidget(m_previewPage);

    // -- Stage 3: Execution --
    m_executionPage = new QWidget();
    QVBoxLayout* execLayout = new QVBoxLayout(m_executionPage);

    QLabel* execTitle = new QLabel("<h3>Execution Status</h3>", m_executionPage);
    execLayout->addWidget(execTitle);

    m_progressBar = new QProgressBar(m_executionPage);
    execLayout->addWidget(m_progressBar);

    m_logView = new QTextEdit(m_executionPage);
    m_logView->setReadOnly(true);
    execLayout->addWidget(m_logView);

    m_btnDone = new QPushButton("Done", m_executionPage);
    m_btnDone->setEnabled(false);
    connect(m_btnDone, &QPushButton::clicked, this, [this](){
        m_stack->setCurrentWidget(m_configPage);
        m_progressBar->setValue(0);
        m_logView->clear();
        m_worker = nullptr; 
    });
    execLayout->addWidget(m_btnDone, 0, Qt::AlignRight);

    m_stack->addWidget(m_executionPage);
}

// Called by derived class in its constructor or logic
// BUT we faced a chicken-and-egg: createConfigWidget is called where?
// We need to call it in constructor but virtuals don't work in constructor.
// Solution: Derived class calls a protected init() method or we call it lazily.
// Let's assume derived class calls init() or wrapper. 
// Actually, we can just let derived class add the widget to m_configPage's layout.
// For now, let's assume `JobBaseWidget` constructor calls `createUI` which uses `createConfigWidget`? NO.
// Let's use a `initialize()` method.

void JobBaseWidget::onPlanClicked() {
    // 1. Create Worker
    m_worker = createWorker();
    if (!m_worker) {
        log("Error: Failed to create worker.");
        return;
    }

    // 2. Set mode to Planning
    m_worker->setMode(Worker::Mode::Planning);

    connect(m_worker, &Worker::planningFinished, this, &JobBaseWidget::onWorkerPlanningFinished);
    connect(m_worker, &Worker::progress, this, &JobBaseWidget::onWorkerProgress);

    // 3. Run in thread pool
    QThreadPool::globalInstance()->start(m_worker);
    
    // Show loading or just wait? For now simple blocking wait or better UI feedback needed?
    // We should probably show a spinner or something. For now, rely on fast planning.
}

void JobBaseWidget::onWorkerPlanningFinished(int, const std::vector<JobItem>& items) {
    m_plannedItems = items;
    buildPreviewTable(items);
    m_stack->setCurrentWidget(m_previewPage);
    
    // Worker finishes after planning and auto-deletes, so m_worker is invalid now?
    // YES, QRunnable autoDelete is true by default.
    // So we need to create a NEW worker for execution.
    m_worker = nullptr; 
}

void JobBaseWidget::buildPreviewTable(const std::vector<JobItem>& items) {
    m_previewTable->setRowCount(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        m_previewTable->setItem(i, 0, new QTableWidgetItem(items[i].sourcePath));
        m_previewTable->setItem(i, 1, new QTableWidgetItem(items[i].destPath));
        m_previewTable->setItem(i, 2, new QTableWidgetItem(items[i].action));
    }
}

void JobBaseWidget::onRunClicked() {
    // Re-create worker for execution
    m_worker = createWorker(); 
    if (!m_worker) return;

    // Inject the planned items? 
    // Usually worker re-calculates or we pass items? 
    // Ideally we pass the items we approved. 
    // Since `createWorker` only takes `Job` struct (via factory or manually), 
    // we need a way to set items.
    // For now, assume re-running is fine OR `createWorker` captures current config state.
    // If strict compliance: we need separate `setItems`.
    // Let's assume implicit config state for now.

    m_worker->setMode(Worker::Mode::Execution);
    m_worker->setItems(m_plannedItems);
    
    connect(m_worker, &Worker::finished, this, &JobBaseWidget::onWorkerFinished);
    connect(m_worker, &Worker::progress, this, &JobBaseWidget::onWorkerProgress);
    connect(m_worker, &Worker::itemCompleted, this, [this](int, int, bool s, const QString& m){
        log(QString("[%1] %2").arg(s ? "OK" : "ERR").arg(m));
    });

    m_stack->setCurrentWidget(m_executionPage);
    m_progressBar->setValue(0);
    QThreadPool::globalInstance()->start(m_worker);
}

void JobBaseWidget::onBackClicked() {
    m_stack->setCurrentWidget(m_configPage);
}

void JobBaseWidget::onWorkerFinished(int, bool success) {
    m_btnDone->setEnabled(true);
    log(success ? "Done!" : "Finished with errors.");
}

void JobBaseWidget::onWorkerProgress(int, int percent, const QString& msg) {
    m_progressBar->setValue(percent);
    // log(msg); // Too spammy?
}

void JobBaseWidget::log(const QString& message) {
    m_logView->append(message);
}
