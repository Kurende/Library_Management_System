#include "MainWindow.h"
#include "DatabaseManager.h"
#include <QApplication>
#include <QMessageBox>
#include <QFile>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application information
    QApplication::setApplicationName("Library Management System");
    QApplication::setOrganizationName("Seagotle Secondary");
    QApplication::setApplicationVersion("1.0.0");

    //set stylesheet
    QFile file(":/theme/themes/dark.qss");

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Failed to open stylesheet!";
    } else {
        QString style = QTextStream(&file).readAll();
        a.setStyleSheet(style);
    }

    
    // Initialize database
    if (!DatabaseManager::instance().initialize("library_system.db")) {
        QMessageBox::critical(nullptr, "Database Error",
                            "Failed to initialize database:\n" +
                            DatabaseManager::instance().getLastError());

        return 1;
    }
    
    // Create and show main window
    MainWindow w;
    w.showMaximized();
    
    return a.exec();
}
