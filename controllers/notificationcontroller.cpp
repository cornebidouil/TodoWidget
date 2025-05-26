/**
 * @file notificationcontroller.cpp
 * @brief Implementation of the NotificationController class
 * 
 * This file implements the NotificationController class which manages task notifications,
 * including due date reminders and overdue warnings. It periodically checks tasks and
 * shows system tray notifications for tasks approaching or past their due dates.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "notificationcontroller.h"
#include <QDateTime>

// Initialize static instance pointer
NotificationController* NotificationController::s_instance = nullptr;

/**
 * @brief Get singleton instance
 * 
 * Returns a reference to the singleton NotificationController instance.
 * Creates the instance if it doesn't exist yet.
 * 
 * @param taskModel Optional task model to use (only used on first call)
 * @param trayIcon Optional system tray icon to use (only used on first call)
 * @return NotificationController& Reference to the singleton instance
 */
NotificationController& NotificationController::instance(TaskModel* taskModel, QSystemTrayIcon* trayIcon)
{
    if (!s_instance) {
        if (!taskModel) {
            taskModel = new TaskModel();
        }
        s_instance = new NotificationController(taskModel, trayIcon);
    } else if (trayIcon && s_instance->m_trayIcon == nullptr) {
        s_instance->setTrayIcon(trayIcon);
    }
    return *s_instance;
}

/**
 * @brief Cleanup the singleton instance
 * 
 * Deletes the singleton instance and sets it to nullptr.
 * Useful for testing and application shutdown.
 */
void NotificationController::cleanup()
{
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

/**
 * @brief Constructor
 * 
 * Creates a new NotificationController associated with the TaskModel and system tray icon.
 * Sets up the timer for periodic checking of task due dates.
 * 
 * @param taskModel Pointer to the TaskModel containing tasks to monitor
 * @param trayIcon Pointer to the QSystemTrayIcon for displaying notifications
 * @param parent Optional parent QObject
 */
NotificationController::NotificationController(TaskModel* taskModel, QSystemTrayIcon* trayIcon, QObject* parent)
    : QObject(parent), m_taskModel(taskModel), m_trayIcon(trayIcon)
{
    // Create and connect timer for regular checking of due tasks
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &NotificationController::onTimerTimeout);
}

/**
 * @brief Destructor
 * 
 * Ensures the timer is stopped before destroying the controller.
 */
NotificationController::~NotificationController()
{
    stop();
}

/**
 * @brief Set the system tray icon
 * 
 * Updates the system tray icon used for notifications.
 * 
 * @param trayIcon Pointer to the QSystemTrayIcon
 */
void NotificationController::setTrayIcon(QSystemTrayIcon* trayIcon)
{
    m_trayIcon = trayIcon;
}

/**
 * @brief Start notification monitoring
 * 
 * Starts the timer that periodically checks for tasks that are due soon or overdue.
 * Only starts if notifications are enabled in the application settings.
 */
void NotificationController::start()
{
    if (!m_timer->isActive() && SettingsManager::instance().enableNotifications()) {
        // Check every 5 minutes (300,000 milliseconds)
        m_timer->start(5 * 60 * 1000);
    }
}

/**
 * @brief Stop notification monitoring
 * 
 * Stops the timer that checks for due tasks, effectively disabling notifications.
 */
void NotificationController::stop()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

/**
 * @brief Check for tasks due soon or overdue
 * 
 * Examines all tasks in the model and shows notifications for:
 * - Tasks due within the next hour that haven't been notified about yet
 * - Tasks that are overdue and haven't been notified about yet
 * 
 * Notifications are only shown if notifications are enabled in settings
 * and the system tray icon is available.
 */
void NotificationController::checkForDueTasks()
{
    // Skip if notifications are disabled or tray icon isn't available
    if (!SettingsManager::instance().enableNotifications() || !m_trayIcon) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QList<Task> tasks = m_taskModel->getTasks();

    for (const Task& task : tasks) {
        // Skip completed tasks
        if (task.isCompleted()) {
            continue;
        }

        // Check for tasks due soon (within the next hour) or overdue
        if (task.dueDate().isValid()) {
            qint64 secsUntilDue = now.secsTo(task.dueDate());

            // Task is due within the next hour and we haven't notified about it yet
            if (secsUntilDue >= 0 && secsUntilDue <= 3600 && !m_notifiedTaskIds.contains(task.id())) {
                m_trayIcon->showMessage(
                    "Task Due Soon",
                    QString("Task \"%1\" is due in %2 minutes.")
                        .arg(task.title())
                        .arg(secsUntilDue / 60),
                    QSystemTrayIcon::Information,
                    10000  // Show the notification for 10 seconds
                );
                // Track that we've notified about this task
                m_notifiedTaskIds.insert(task.id());
            }
            // Task is overdue and we haven't notified about it yet
            else if (secsUntilDue < 0 && !m_notifiedTaskIds.contains(task.id() + "_overdue")) {
                m_trayIcon->showMessage(
                    "Task Overdue",
                    QString("Task \"%1\" is overdue by %2 hours.")
                        .arg(task.title())
                        .arg((-secsUntilDue) / 3600),
                    QSystemTrayIcon::Warning,
                    10000  // Show the notification for 10 seconds
                );
                // Track that we've notified about this overdue task
                m_notifiedTaskIds.insert(task.id() + "_overdue");
            }
        }
    }
}

/**
 * @brief Timer timeout slot
 * 
 * Called when the notification timer times out.
 * Triggers a check for tasks that are due soon or overdue.
 */
void NotificationController::onTimerTimeout()
{
    checkForDueTasks();
}
