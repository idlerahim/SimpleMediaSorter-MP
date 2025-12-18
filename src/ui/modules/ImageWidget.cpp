#include "ImageWidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QJsonObject>
#include "../../workers/ImageWorker.h"
#include "../../core/Job.h"

ImageWidget::ImageWidget(QWidget *parent) : JobBaseWidget(parent) {
    QWidget* config = createConfigWidget();
    QVBoxLayout* l = new QVBoxLayout(m_configPage);
    l->addWidget(config);
}

void ImageWidget::init() {}

QWidget* ImageWidget::createConfigWidget() {
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    
    layout->addWidget(new QLabel("<h3>Bulk Image Processor</h3>"));
    layout->addWidget(new QLabel("Resize and convert images in batch."));

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

    // Options
    m_widthSpin = new QSpinBox();
    m_widthSpin->setRange(0, 10000);
    m_widthSpin->setValue(0); // 0 = Keep original
    m_widthSpin->setSuffix(" px");
    m_widthSpin->setSpecialValueText("Original Width");

    m_qualitySpin = new QSpinBox();
    m_qualitySpin->setRange(1, 100);
    m_qualitySpin->setValue(90);

    m_formatCombo = new QComboBox();
    m_formatCombo->addItems({"jpg", "png", "webp", "bmp"});

    form->addRow("Source:", srcLayout);
    form->addRow("Output:", destLayout);
    form->addRow("Resize Width:", m_widthSpin);
    form->addRow("Quality:", m_qualitySpin);
    form->addRow("Format:", m_formatCombo);

    layout->addLayout(form);
    layout->addStretch();
    
    QPushButton* btnPlan = new QPushButton("Preview Plan");
    connect(btnPlan, &QPushButton::clicked, this, [this](){ onPlanClicked(); });
    layout->addWidget(btnPlan);
    
    return w;
}

Worker* ImageWidget::createWorker() {
    Job job;
    job.type = "IMAGE"; 
    QJsonObject params;
    params["source"] = m_srcEdit->text();
    params["dest"] = m_destEdit->text();
    params["width"] = m_widthSpin->value();
    params["quality"] = m_qualitySpin->value();
    params["format"] = m_formatCombo->currentText();
    job.params = params;
    
    return new ImageWorker(job);
}
