#include "MainWindow.h"
#include "DatabaseManager.h"
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application information
    QApplication::setApplicationName("Library Management System");
    QApplication::setOrganizationName("Makhutswe/Lepelle Circuits");
    QApplication::setApplicationVersion("1.0.0");

    //set stylesheet
    QFile file(":Style/stylesheet.qss");

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
