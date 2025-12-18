#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUi();
    void setupConnections();

    QListWidget* m_sidebar;
    QStackedWidget* m_contentArea;
};
