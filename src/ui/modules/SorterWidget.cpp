#include "SorterWidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QJsonObject>
#include "../../workers/SorterWorker.h"
#include "../../core/Job.h"

SorterWidget::SorterWidget(QWidget *parent) : JobBaseWidget(parent) {
    QWidget* config = createConfigWidget();
    QVBoxLayout* l = new QVBoxLayout(m_configPage);
    l->addWidget(config);
}

void SorterWidget::init() {}

QWidget* SorterWidget::createConfigWidget() {
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    
    layout->addWidget(new QLabel("<h3>Date-Based Sorter</h3>"));
    layout->addWidget(new QLabel("Sort files into folders based on their creation/modification date."));

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

    // Format
    m_formatCombo = new QComboBox();
    m_formatCombo->addItems({"yyyy/MM/dd", "yyyy/MM", "yyyy-MM-dd", "yyyy"});
    m_formatCombo->setEditable(true);

    form->addRow("Source Folder:", srcLayout);
    form->addRow("Destination Folder:", destLayout);
    form->addRow("Folder Structure:", m_formatCombo);
    
    layout->addLayout(form);
    layout->addStretch();
    
    QPushButton* btnPlan = new QPushButton("Preview Plan");
    connect(btnPlan, &QPushButton::clicked, this, [this](){ onPlanClicked(); });
    layout->addWidget(btnPlan);
    
    return w;
}

Worker* SorterWidget::createWorker() {
    Job job;
    job.type = "SORT"; 
    QJsonObject params;
    params["source"] = m_srcEdit->text();
    params["dest"] = m_destEdit->text();
    params["format"] = m_formatCombo->currentText();
    job.params = params;
    
    return new SorterWorker(job);
}
