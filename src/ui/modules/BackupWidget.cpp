#include "BackupWidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QJsonObject>
#include "../../workers/CopyWorker.h"
#include "../../core/Job.h"

BackupWidget::BackupWidget(QWidget *parent) : JobBaseWidget(parent) {
    QWidget* config = createConfigWidget();
    QVBoxLayout* l = new QVBoxLayout(m_configPage);
    l->addWidget(config);
}

void BackupWidget::init() {}

QWidget* BackupWidget::createConfigWidget() {
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    
    layout->addWidget(new QLabel("<h3>Backup / Copy Structure</h3>"));
    layout->addWidget(new QLabel("Copy files from Source to Destination while preserving folder structure."));

    QFormLayout* form = new QFormLayout();
    
    // Source
    m_srcEdit = new QLineEdit();
    QPushButton* btnSrc = new QPushButton("Browse...");
    connect(btnSrc, &QPushButton::clicked, this, [this](){
        QString dir = QFileDialog::getExistingDirectory(this, "Select Source Directory");
        if (!dir.isEmpty()) m_srcEdit->setText(dir);
    });
    QHBoxLayout* srcLayout = new QHBoxLayout();
    srcLayout->addWidget(m_srcEdit);
    srcLayout->addWidget(btnSrc);
    
    // Dest
    m_destEdit = new QLineEdit();
    QPushButton* btnDest = new QPushButton("Browse...");
    connect(btnDest, &QPushButton::clicked, this, [this](){
        QString dir = QFileDialog::getExistingDirectory(this, "Select Destination Directory");
        if (!dir.isEmpty()) m_destEdit->setText(dir);
    });
    QHBoxLayout* destLayout = new QHBoxLayout();
    destLayout->addWidget(m_destEdit);
    destLayout->addWidget(btnDest);

    form->addRow("Source Folder:", srcLayout);
    form->addRow("Destination Folder:", destLayout);
    
    layout->addLayout(form);
    layout->addStretch();
    
    // Action buttons
    QPushButton* btnPlan = new QPushButton("Preview Plan");
    connect(btnPlan, &QPushButton::clicked, this, [this](){ onPlanClicked(); });
    layout->addWidget(btnPlan);
    
    return w;
}

Worker* BackupWidget::createWorker() {
    Job job;
    job.type = "COPY"; // Must match logic in factory if used, or just worker logic
    QJsonObject params;
    params["source"] = m_srcEdit->text();
    params["dest"] = m_destEdit->text();
    job.params = params;
    
    return new CopyWorker(job);
}
