#include "JobDetailsPane.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QJsonDocument>

JobDetailsPane::JobDetailsPane(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();
    m_lblId = new QLabel("-");
    m_lblType = new QLabel("-");
    m_lblState = new QLabel("-");
    m_lblProgress = new QLabel("-");
    
    formLayout->addRow("ID:", m_lblId);
    formLayout->addRow("Type:", m_lblType);
    formLayout->addRow("State:", m_lblState);
    formLayout->addRow("Progress:", m_lblProgress);

    mainLayout->addLayout(formLayout);

    mainLayout->addWidget(new QLabel("Parameters:"));
    m_txtParams = new QTextEdit();
    m_txtParams->setReadOnly(true);
    mainLayout->addWidget(m_txtParams);

    mainLayout->addWidget(new QLabel("Logs:"));
    m_txtLogs = new QTextEdit();
    m_txtLogs->setReadOnly(true);
    mainLayout->addWidget(m_txtLogs);
}

void JobDetailsPane::setJob(const Job& job) {
    m_lblId->setText(QString::number(job.id));
    m_lblType->setText(job.type);
    
    switch(job.state) {
        case JobState::Pending: m_lblState->setText("Pending"); break;
        case JobState::Running: m_lblState->setText("Running"); break;
        case JobState::Finished: m_lblState->setText("Finished"); break;
        case JobState::Failed: m_lblState->setText("Failed"); break;
        default: m_lblState->setText("Unknown"); break;
    }

    m_lblProgress->setText(QString("%1%").arg(job.progress));
    m_txtParams->setText(QJsonDocument(job.params).toJson(QJsonDocument::Indented));
    m_txtLogs->setText(job.statusMessage);
}

void JobDetailsPane::clear() {
    m_lblId->setText("-");
    m_lblType->setText("-");
    m_lblState->setText("-");
    m_lblProgress->setText("-");
    m_txtParams->clear();
    m_txtLogs->clear();
}
