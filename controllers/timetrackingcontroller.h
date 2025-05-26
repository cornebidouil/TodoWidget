/**
 * @file timetrackingcontroller.h
 * @brief Definition of the TimeTrackingController class
 * 
 * This file defines the TimeTrackingController class which manages time tracking operations
 * in the application. It provides methods for starting and stopping timers, managing time entries,
 * and generating time reports.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include "../models/timeentry.h"
#include "../models/timeentrymodel.h"

/**
 * @class TimeTrackingController
 * @brief Controller for time tracking operations
 * 
 * The TimeTrackingController class manages all time tracking operations in the application.
 * It provides methods for starting and stopping timers, managing time entries, and generating
 * time reports. It is implemented as a singleton to ensure there is only one instance
 * throughout the application.
 */
class TimeTrackingController : public QObject {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor to enforce singleton pattern
     * @param parent Optional parent object
     */
    explicit TimeTrackingController(TimeEntryModel *model, QObject* parent = nullptr);

    /**
     * @brief Destructor to enforce singleton pattern
     */
    ~TimeTrackingController();

    /**
     * @brief Get the singleton instance
     * @return TimeTrackingController& Reference to the singleton instance
     */
    static TimeTrackingController& instance(TimeEntryModel *model = nullptr);
    
    /**
     * @brief Cleanup the singleton instance
     * 
     * Deletes the singleton instance and sets it to nullptr.
     * Useful for testing and application shutdown.
     */
    static void cleanup();
    
    /**
     * @brief Initialize the controller
     * @return bool True if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Get the time entry model
     * @return TimeEntryModel* Pointer to the time entry model
     */
    TimeEntryModel* timeEntryModel() const { return m_timeEntryModel; }
    
    /**
     * @brief Start a timer for a project
     * 
     * Starts tracking time for the specified project. If there's already a timer running,
     * it stops the current one before starting the new one.
     * 
     * @param projectId The ID of the project to track
     * @return bool True if the timer was started, false otherwise
     */
    bool startTimer(const QString& projectId);
    
    /**
     * @brief Stop the current timer
     * 
     * Stops the currently running timer and creates a time entry for the tracked time.
     * 
     * @return bool True if a timer was running and stopped, false otherwise
     */
    bool stopTimer();
    
    /**
     * @brief Check if a timer is currently running
     * @return bool True if a timer is running, false otherwise
     */
    bool isTimerRunning() const;
    
    /**
     * @brief Get the ID of the currently tracked project
     * @return QString The ID of the project being tracked, or an empty string if no timer is running
     */
    QString getCurrentProjectId() const;
    
    /**
     * @brief Get the elapsed time for the current timer
     * @return int The elapsed time in seconds, or 0 if no timer is running
     */
    int getCurrentElapsed() const;
    
    /**
     * @brief Add a time entry
     * 
     * Adds a new time entry to the model and persists it to the database.
     * 
     * @param entry The time entry to add
     * @return bool True if the entry was added successfully, false otherwise
     */
    bool addTimeEntry(const TimeEntry& entry);
    
    /**
     * @brief Update a time entry
     * 
     * Updates an existing time entry in the model and persists the changes to the database.
     * 
     * @param entry The time entry to update
     * @return bool True if the entry was updated successfully, false otherwise
     */
    bool updateTimeEntry(const TimeEntry& entry);
    
    /**
     * @brief Delete a time entry
     * 
     * Removes a time entry from the model and deletes it from the database.
     * 
     * @param id The ID of the entry to delete
     * @return bool True if the entry was deleted successfully, false otherwise
     */
    bool deleteTimeEntry(const QString& id);
    
    /**
     * @brief Get time entries for a date range
     * 
     * Retrieves all time entries that fall within the specified date range.
     * 
     * @param startDate Start date of the range
     * @param endDate End date of the range
     * @return QList<TimeEntry> List of time entries in the range
     */
    QList<TimeEntry> getTimeEntries(const QDate& startDate, const QDate& endDate) const;
    
    /**
     * @brief Get today's total tracked time
     * @return int Total duration in seconds
     */
    int getTodayTotal() const;
    
    /**
     * @brief Get this week's total tracked time
     * @return int Total duration in seconds
     */
    int getWeekTotal() const;
    
    /**
     * @brief Get the most tracked project
     * 
     * Determines which project has the most tracked time within a specified date range.
     * 
     * @param startDate Start date of the range
     * @param endDate End date of the range
     * @return QString The ID of the project with the most tracked time
     */
    QString getMostTrackedProject(const QDate& startDate, const QDate& endDate) const;
    
    /**
     * @brief Get total time by project
     * 
     * Gets a mapping of project IDs to their total tracked time within a specified date range.
     * 
     * @param startDate Start date of the range
     * @param endDate End date of the range
     * @return QMap<QString, int> Map of project IDs to total durations in seconds
     */
    QMap<QString, int> getTimeByProject(const QDate& startDate, const QDate& endDate) const;
    
    /**
     * @brief Get total time by day
     * 
     * Gets a mapping of dates to their total tracked time within a specified date range.
     * 
     * @param startDate Start date of the range
     * @param endDate End date of the range
     * @return QMap<QDate, int> Map of dates to total durations in seconds
     */
    QMap<QDate, int> getTimeByDay(const QDate& startDate, const QDate& endDate) const;
    
    /**
     * @brief Load all time entries from the database
     * @return bool True if entries were loaded successfully, false otherwise
     */
    bool loadTimeEntries();
    
    /**
     * @brief Save all time entries to the database
     * @return bool True if entries were saved successfully, false otherwise
     */
    bool saveTimeEntries();
    
    /**
     * @brief Format a duration in seconds as a string
     * @param seconds Duration in seconds
     * @param format Format string (e.g. "hh:mm:ss" or "h:mm")
     * @return QString Formatted duration string
     */
    static QString formatDuration(int seconds, const QString& format = "hh:mm:ss");

signals:
    /**
     * @brief Emitted when a timer is started
     * @param projectId The ID of the project being tracked
     */
    void timerStarted(const QString& projectId);
    
    /**
     * @brief Emitted when a timer is stopped
     * @param duration The duration of the tracked time in seconds
     */
    void timerStopped(int duration);
    
    /**
     * @brief Emitted periodically while a timer is running
     * @param elapsed The elapsed time in seconds
     */
    void timerTick(int elapsed);
    
    /**
     * @brief Emitted when a time entry is added
     * @param entry The added time entry
     */
    void timeEntryAdded(const TimeEntry& entry);
    
    /**
     * @brief Emitted when a time entry is updated
     * @param entry The updated time entry
     */
    void timeEntryUpdated(const TimeEntry& entry);
    
    /**
     * @brief Emitted when a time entry is deleted
     * @param id The ID of the deleted time entry
     */
    void timeEntryDeleted(const QString& id);

private slots:
    /**
     * @brief Handle timer tick
     * 
     * Called every second while a timer is running. Updates the elapsed time
     * and emits the timerTick signal.
     */
    void onTimerTick();

private:
    /**
     * @brief Private copy constructor to enforce singleton pattern
     */
    TimeTrackingController(const TimeTrackingController&) = delete;
    
    /**
     * @brief Private assignment operator to enforce singleton pattern
     */
    TimeTrackingController& operator=(const TimeTrackingController&) = delete;

    QTimer* m_timer;                  ///< Timer for tracking time
    QDateTime m_startTime;            ///< Start time of the current timer
    QString m_currentProjectId;       ///< ID of the project being tracked
    TimeEntryModel* m_timeEntryModel; ///< Model for time entries
    bool m_initialized;               ///< Flag indicating if the controller is initialized
    static TimeTrackingController *s_instance; ///< Singleton instance
};
