#include "MainWindow.h"
#include "components/SidebarWidget.h"
#include "modules/RenamerWidget.h"
#include "modules/SorterWidget.h"
#include "modules/BackupWidget.h"
#include "modules/ImageWidget.h"
#include "modules/VideoWidget.h"
#include <QHBoxLayout>
#include <QStatusBar>
#include <QLabel>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    resize(1200, 800);
    setWindowTitle("SMSorter - Premium Media Manager");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* layout = new QHBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Sidebar
    auto sidebar = new SidebarWidget(this); 
    sidebar->addDestination("Home", "");
    sidebar->addDestination("Renamer", "");
    sidebar->addDestination("Sorter", "");
    sidebar->addDestination("Backup", "");
    sidebar->addDestination("Images", "");
    sidebar->addDestination("Video", "");
    sidebar->setFixedWidth(250);
    
    m_sidebar = sidebar; 
    layout->addWidget(m_sidebar);

    // Content Area
    m_contentArea = new QStackedWidget(this);
    layout->addWidget(m_contentArea);

    // Modules
    // 0: Home
    QLabel* homeLabel = new QLabel("<h1 style='color:#ccc'>Welcome to SMSorter</h1><p style='color:#aaa'>Select a module from the sidebar to begin.</p>", this);
    homeLabel->setAlignment(Qt::AlignCenter);
    homeLabel->setStyleSheet("background-color: #1e1e1e;");
    
    // Logo
    QPixmap logo("MediaSorter.png");
    if (!logo.isNull()) {
        QLabel* logoLbl = new QLabel(this);
        logoLbl->setPixmap(logo.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLbl->setAlignment(Qt::AlignCenter);
        
        QVBoxLayout* homeLayout = new QVBoxLayout();
        homeLayout->addStretch();
        homeLayout->addWidget(logoLbl);
        homeLayout->addWidget(homeLabel);
        homeLayout->addStretch();
        
        QWidget* homeWidget = new QWidget();
        homeWidget->setLayout(homeLayout);
        m_contentArea->addWidget(homeWidget);
        setWindowIcon(QIcon("MediaSorter.png"));
    } else {
        m_contentArea->addWidget(homeLabel);
    }

    // 1: Renamer
    m_contentArea->addWidget(new RenamerWidget(this));

    // 2: Sorter
    m_contentArea->addWidget(new SorterWidget(this));

    // 3: Backup
    m_contentArea->addWidget(new BackupWidget(this));

    // 4: Image
    m_contentArea->addWidget(new ImageWidget(this));

    // 5: Video
    m_contentArea->addWidget(new VideoWidget(this));
    
    // About
    sidebar->addDestination("About", ""); 
    
    // About Page
    QLabel* aboutLabel = new QLabel(
        "<h2>SMSorter</h2>"
        "<p>Version 1.0.0.5</p>"
        "<p>State-of-the-art Media Management Suite.</p>"
        "<p>Copyright 2025</p>", 
        this);
    aboutLabel->setAlignment(Qt::AlignCenter);
    m_contentArea->addWidget(aboutLabel);

    setupConnections();
    
    // Status Bar
    QStatusBar* status = statusBar();
    status->showMessage("Ready");
}

void MainWindow::setupConnections() {
    connect(m_sidebar, &QListWidget::currentRowChanged, m_contentArea, &QStackedWidget::setCurrentIndex);
}
