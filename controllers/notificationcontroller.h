/**
 * @file notificationcontroller.h
 * @brief Definition of the NotificationController class
 * 
 * This file defines the NotificationController class which manages task due date
 * notifications via the system tray. It monitors tasks and sends notifications
 * when they are approaching their due date.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QObject>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QSet>
#include "../models/taskmodel.h"
#include "../services/settingsmanager.h"

/**
 * @class NotificationController
 * @brief Controller for task notifications
 * 
 * The NotificationController class monitors tasks with due dates and shows
 * system tray notifications when tasks are approaching or past their due dates.
 * It uses a timer to periodically check for tasks that need notifications
 * and keeps track of which tasks have already been notified about to avoid
 * duplicate notifications.
 */
class NotificationController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new NotificationController connected to the specified TaskModel
     * and system tray icon.
     * 
     * @param taskModel Pointer to the TaskModel to monitor
     * @param trayIcon Pointer to the QSystemTrayIcon to show notifications with
     * @param parent Optional parent QObject
     */
    explicit NotificationController(TaskModel* taskModel, QSystemTrayIcon* trayIcon, QObject* parent = nullptr);

    /**
     * @brief Start notification monitoring
     * 
     * Begins the periodic checking for tasks that need notifications.
     */
    void start();
    
    /**
     * @brief Stop notification monitoring
     * 
     * Stops the periodic checking for tasks that need notifications.
     */
    void stop();

    /**
     * @brief Check for tasks that need notifications
     * 
     * Scans all tasks and shows notifications for those approaching or past
     * their due dates, according to the current notification settings.
     */
    void checkForDueTasks();

private slots:
    /**
     * @brief Handle timer timeout
     * 
     * Called when the monitoring timer expires to check for due tasks.
     */
    void onTimerTimeout();

private:
    TaskModel* m_taskModel;            ///< Pointer to the task model being monitored
    QSystemTrayIcon* m_trayIcon;       ///< Pointer to the system tray icon for showing notifications
    QTimer* m_timer;                   ///< Timer for periodic checking
    QSet<QString> m_notifiedTaskIds;   ///< Set of IDs of tasks already notified about
};
