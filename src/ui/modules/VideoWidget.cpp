#include "VideoWidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QJsonObject>
#include "../../workers/FFmpegWorker.h"
#include "../../core/Job.h"

VideoWidget::VideoWidget(QWidget *parent) : JobBaseWidget(parent) {
    QWidget* config = createConfigWidget();
    QVBoxLayout* l = new QVBoxLayout(m_configPage);
    l->addWidget(config);
}

void VideoWidget::init() {}

QWidget* VideoWidget::createConfigWidget() {
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    
    layout->addWidget(new QLabel("<h3>FFMPEG Video Transcoder</h3>"));
    layout->addWidget(new QLabel("Batch convert videos using FFmpeg."));

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

    // Codec
    m_codecCombo = new QComboBox();
    m_codecCombo->addItems({"copy", "libx264", "libx265", "prores"});
    
    m_containerCombo = new QComboBox();
    m_containerCombo->addItems({"mp4", "mkv", "mov", "avi"});

    form->addRow("Source Folder:", srcLayout);
    form->addRow("Output Folder:", destLayout);
    form->addRow("Video Codec:", m_codecCombo);
    form->addRow("Container (Output):", m_containerCombo);
    
    layout->addLayout(form);
    layout->addStretch();
    
    QPushButton* btnPlan = new QPushButton("Preview Plan");
    connect(btnPlan, &QPushButton::clicked, this, [this](){ onPlanClicked(); });
    layout->addWidget(btnPlan);
    
    return w;
}

Worker* VideoWidget::createWorker() {
    Job job;
    job.type = "FFMPEG"; 
    QJsonObject params;
    params["source"] = m_srcEdit->text();
    params["dest"] = m_destEdit->text();
    params["codec"] = m_codecCombo->currentText();
    params["container"] = m_containerCombo->currentText();
    job.params = params;
    
    return new FFmpegWorker(job);
}
