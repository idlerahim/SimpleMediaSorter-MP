#include "RenamerWidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QJsonObject>
#include <QLabel>
#include "../../workers/MetadataWorker.h"
#include "../../core/Job.h"

RenamerWidget::RenamerWidget(QWidget *parent) : JobBaseWidget(parent) {
    QWidget* config = createConfigWidget();
    QVBoxLayout* l = new QVBoxLayout(m_configPage);
    l->addWidget(config);
}

void RenamerWidget::init() {
    // No-op or remove if not needed
}

QWidget* RenamerWidget::createConfigWidget() {
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    
    // File selection
    m_filesList = new QListWidget();
    QPushButton* btnAdd = new QPushButton("Add Files");
    connect(btnAdd, &QPushButton::clicked, this, [this](){
        QStringList files = QFileDialog::getOpenFileNames(this, "Select Files");
        m_filesList->addItems(files);
    });
    
    layout->addWidget(new QLabel("Files:"));
    layout->addWidget(m_filesList);
    layout->addWidget(btnAdd);

    // Options
    QFormLayout* form = new QFormLayout();
    m_patternEdit = new QLineEdit("IMG_{YYYY}{MM}{DD}_{MEASURE}");
    m_regexCheck = new QCheckBox("Use Regex");
    m_destEdit = new QLineEdit(); // Optional output folder
    
    form->addRow("Pattern:", m_patternEdit);
    form->addRow("", m_regexCheck);
    form->addRow("Output Folder (Optional):", m_destEdit);
    
    layout->addLayout(form);
    
    // Action buttons (Plan) handled by base
    QPushButton* btnPlan = new QPushButton("Preview Plan");
    connect(btnPlan, &QPushButton::clicked, this, [this](){ onPlanClicked(); });
    layout->addWidget(btnPlan);
    
    return w;
}

Worker* RenamerWidget::createWorker() {
    // Collect params
    Job job;
    job.type = "RENAME";
    QJsonObject params;
    params["pattern"] = m_patternEdit->text();
    params["regex"] = m_regexCheck->isChecked();
    params["dest"] = m_destEdit->text();
    
    // Get items
    for(int i=0; i<m_filesList->count(); ++i) {
        JobItem item;
        item.sourcePath = m_filesList->item(i)->text();
        job.items.push_back(item);
    }
    job.params = params;

    return new MetadataWorker(job); // Implementation of plan() needed in MetadataWorker
}
