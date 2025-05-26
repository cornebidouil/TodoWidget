/**
 * @file main.cpp
 * @brief Application entry point for the TODO Widget application
 * 
 * This file contains the main function and application initialization code.
 * It handles:
 * - Application setup (name, organization, icon)
 * - Single instance enforcement using QLocalServer/QLocalSocket
 * - Custom message handling for logging
 * - Stylesheet loading
 * - Database initialization
 * - Settings loading
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSharedMemory>
#include "controllers/taskcontroller.h"
#include "controllers/categorycontroller.h"
#include "controllers/projectcontroller.h"
#include "controllers/timetrackingcontroller.h"
#include "controllers/notificationcontroller.h"
#include "views/mainwindow.h"

/**
 * @brief Clean up all singleton instances
 * 
 * Calls cleanup on all controller singletons to properly release resources.
 * This should be called at application shutdown.
 */
void cleanupSingletons()
{
    TaskController::cleanup();
    CategoryController::cleanup();
    ProjectController::cleanup();
    TimeTrackingController::cleanup();
    NotificationController::cleanup();
}

/**
 * @brief Custom message handler for Qt logging
 * 
 * Redirects Qt's logging messages to a file in the application data directory
 * while preserving output to the console. Formats log entries with timestamps
 * and contextual information based on message severity.
 *
 * @param type The severity level of the message
 * @param context Additional context information about where the message originated
 * @param msg The actual message text
 */
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Get the application data location for storing logs
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir logDir(logPath);

    // Create directory if it doesn't exist
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }

    // Create or open log file (append mode)
    QFile logFile(logDir.filePath("todowidget.log"));
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        // Fall back to standard output if file can't be opened
        fprintf(stderr, "Warning: Could not open log file for writing\n");
        return;
    }

    // Format current date and time
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    // Format based on message type
    QString logEntry;
    switch (type) {
    case QtDebugMsg:
        logEntry = QString("[%1] [Debug] %2").arg(timestamp, msg);
        break;
    case QtInfoMsg:
        logEntry = QString("[%1] [Info] %2").arg(timestamp, msg);
        break;
    case QtWarningMsg:
        logEntry = QString("[%1] [Warning] %2 (%3:%4, %5)").arg(
            timestamp, msg,
            QString(context.file ? context.file : "unknown"),
            QString::number(context.line),
            QString(context.function ? context.function : "unknown"));
        break;
    case QtCriticalMsg:
        logEntry = QString("[%1] [Critical] %2 (%3:%4, %5)").arg(
            timestamp, msg,
            QString(context.file ? context.file : "unknown"),
            QString::number(context.line),
            QString(context.function ? context.function : "unknown"));
        break;
    case QtFatalMsg:
        logEntry = QString("[%1] [Fatal] %2 (%3:%4, %5)").arg(
            timestamp, msg,
            QString(context.file ? context.file : "unknown"),
            QString::number(context.line),
            QString(context.function ? context.function : "unknown"));
        break;
    }

    // Write to log file
    logEntry += "\n";
    logFile.write(logEntry.toUtf8());
    logFile.flush();

    // Also output to console for development purposes
    fprintf(stderr, "%s\n", qPrintable(logEntry));

    // In case of fatal error, flush and close before aborting
    if (type == QtFatalMsg) {
        logFile.close();
        abort();
    }
}

/**
 * @brief Application entry point
 * 
 * Sets up the application environment, handles single-instance enforcement,
 * initializes resources, and launches the main window.
 *
 * @param argc Command line argument count
 * @param argv Command line argument values
 * @return int Application exit code
 */
int main(int argc, char *argv[])
{
    // Set up logging
    //qInstallMessageHandler(customMessageHandler);

    qInfo() << "Application starting up";

    QApplication app(argc, argv);

    // Set application information
    qInfo() << "Setting up application information";
    app.setApplicationName("TODO Widget");
    app.setOrganizationName("TODO Widget");
    app.setOrganizationDomain("todowidget.example.com");

    // Set application icon
    qInfo() << "Setting application icon";
    app.setWindowIcon(QIcon(":/icons/app_icon.svg"));

    // Check for existing instance
    const QString serverName = "TodoWidgetSingleInstanceServer";
    QLocalSocket socket;
    socket.connectToServer(serverName);

    if (socket.waitForConnected(500)) {
        // An existing instance is running
        qInfo() << "An instance is already running. Sending show command.";
        socket.write("SHOW");
        socket.flush();
        socket.waitForBytesWritten(1000);
        socket.close();
        return 0; // Exit this instance
    }

    // No existing instance found, continue with normal startup

    // Create local server for future instances
    QLocalServer *server = new QLocalServer(&app);
    QObject::connect(server, &QLocalServer::newConnection, [server]() {
        QLocalSocket *socket = server->nextPendingConnection();
        if (socket->waitForReadyRead(1000)) {
            QByteArray message = socket->readAll();
            if (message == "SHOW") {
                // Find the main window and show it
                for (QWidget *widget : QApplication::topLevelWidgets()) {
                    if (MainWindow *mainWindow = qobject_cast<MainWindow*>(widget)) {
                        //mainWindow->show();
                        //mainWindow->activateWindow();
                        //mainWindow->raise();
                        mainWindow->showAndActivate();
                        break;
                    }
                }
            }
        }
        socket->close();
        socket->deleteLater();
    });

    // Make sure the server name is not already in use
    QLocalServer::removeServer(serverName);
    if (!server->listen(serverName)) {
        qWarning() << "Could not create local server:" << server->errorString();
    }

    // Load application stylesheet
    qInfo() << "Loading stylesheet";
    QFile styleFile(":/styles/app_style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    // Initialize database
    qInfo() << "Initializing database";
    if (!DatabaseManager::instance().initialize()) {
        qCritical() << "Database initialization failed";
        return 1;
    }

    // Load settings
    qInfo() << "Loading settings";
    SettingsManager::instance().load();

    // Create and show main window
    qInfo() << "Creating main window";
    MainWindow mainWindow;

    // Show the window if not starting minimized
    qInfo() << "Showing main window";
    if (!SettingsManager::instance().startMinimized()) {
        mainWindow.show();
    }

    qInfo() << "Entering application event loop";
    int result = app.exec();
    
    // Clean up singletons before exit
    cleanupSingletons();
    
    return result;
}
