#include <QApplication>
#include "ui/MainWindow.h"
#include "core/DatabaseManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Initialize Database
    if (!DatabaseManager::instance().init()) {
        // Handle error - maybe show a message box
        return 1;
    }

    MainWindow window;
    window.show();

    return app.exec();
}
