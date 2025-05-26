/**
 * @file timetrackingcontroller.cpp
 * @brief Implementation of the TimeTrackingController class
 * 
 * The TimeTrackingController manages all time tracking operations in the application.
 * It handles starting and stopping timers, adding, updating, and deleting time entries,
 * and calculating various time-related statistics. This controller implements a singleton
 * pattern to ensure a single point of access for time tracking functionality throughout
 * the application.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "timetrackingcontroller.h"
#include "../services/databasemanager.h"
#include "../controllers/projectcontroller.h"
#include <QDebug>

// Initialize static instance pointer
TimeTrackingController* TimeTrackingController::s_instance = nullptr;

/**
 * @brief Constructor for TimeTrackingController
 *
 * Initializes the controller with the provided time entry model and sets up
 * the timer for tracking elapsed time.
 *
 * @param model The TimeEntryModel to be managed by this controller
 * @param parent The parent QObject (optional)
 */
TimeTrackingController::TimeTrackingController(TimeEntryModel *model, QObject* parent)
    : QObject(parent)
    , m_timeEntryModel(model)
    , m_timer(new QTimer(this))
    , m_startTime(QDateTime())
    , m_currentProjectId("")
    , m_initialized(false)
{
    // Set up the timer to fire every second
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &TimeTrackingController::onTimerTick);
}

/**
 * @brief Destructor for TimeTrackingController
 *
 * Ensures any running timer is stopped before destruction.
 */
TimeTrackingController::~TimeTrackingController()
{
    // Ensure any running timer is stopped before destruction
    if (isTimerRunning()) {
        stopTimer();
    }
}

/**
 * @brief Get the singleton instance of TimeTrackingController
 *
 * Creates a new instance if one doesn't exist yet.
 * 
 * @param model Optional TimeEntryModel to use; if null, a new model is created
 * @return Reference to the singleton instance
 */
TimeTrackingController& TimeTrackingController::instance(TimeEntryModel *model)
{
    if (!s_instance) {
        if (!model) {
            model = new TimeEntryModel();
        }
        s_instance = new TimeTrackingController(model);
    }
    
    return *s_instance;
}

/**
 * @brief Cleanup the singleton instance
 * 
 * Deletes the singleton instance and sets it to nullptr.
 * Useful for testing and application shutdown.
 */
void TimeTrackingController::cleanup()
{
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

/**
 * @brief Initialize the controller
 *
 * Loads time entries from the database and sets the initialized flag.
 * This should be called once after creating the controller.
 *
 * @return true if initialization was successful, false otherwise
 */
bool TimeTrackingController::initialize()
{
    if (m_initialized) {
        return true;
    }
    
    // Load time entries from the database
    bool success = loadTimeEntries();
    
    if (success) {
        m_initialized = true;
        qDebug() << "TimeTrackingController initialized successfully";
    } else {
        qWarning() << "Failed to initialize TimeTrackingController";
    }
    
    return success;
}

/**
 * @brief Start the timer for tracking time on a project
 *
 * If a timer is already running, it stops it first and creates a time entry.
 * Then starts tracking time for the specified project.
 *
 * @param projectId The ID of the project to track time for
 * @return true if the timer was started successfully, false otherwise
 */
bool TimeTrackingController::startTimer(const QString& projectId)
{
    // If there's already a timer running, stop it first
    if (isTimerRunning()) {
        stopTimer();
    }
    
    // Check if the project exists
    if (ProjectController::instance().getProject(projectId).id().isEmpty()) {
        qWarning() << "Cannot start timer: Project with ID" << projectId << "not found";
        return false;
    }
    
    // Start tracking the new project
    m_currentProjectId = projectId;
    m_startTime = QDateTime::currentDateTime();
    m_timer->start();
    
    emit timerStarted(projectId);
    qDebug() << "Started timer for project:" << projectId;
    
    return true;
}

/**
 * @brief Stop the timer and create a time entry
 *
 * Stops the active timer, calculates the duration, and creates a new
 * time entry for the tracked time.
 *
 * @return true if the timer was stopped successfully, false otherwise
 */
bool TimeTrackingController::stopTimer()
{
    if (!isTimerRunning()) {
        return false;
    }
    
    // Stop the timer
    m_timer->stop();
    
    // Create a time entry for the tracked time
    QDateTime endTime = QDateTime::currentDateTime();
    int duration = m_startTime.secsTo(endTime);
    
    TimeEntry entry(m_currentProjectId, m_startTime, endTime, duration);
    addTimeEntry(entry);
    
    // Clear the current tracking state
    QString projectId = m_currentProjectId;
    m_currentProjectId = "";
    m_startTime = QDateTime();
    
    emit timerStopped(duration);
    qDebug() << "Stopped timer for project:" << projectId << "Duration:" << duration << "seconds";
    
    return true;
}

/**
 * @brief Check if the timer is currently running
 *
 * @return true if the timer is active and tracking a project, false otherwise
 */
bool TimeTrackingController::isTimerRunning() const
{
    return m_timer->isActive() && !m_currentProjectId.isEmpty();
}

/**
 * @brief Get the ID of the project currently being tracked
 *
 * @return The ID of the current project, or empty string if not tracking
 */
QString TimeTrackingController::getCurrentProjectId() const
{
    return m_currentProjectId;
}

/**
 * @brief Get the elapsed time for the current tracking session
 *
 * @return The elapsed time in seconds, or 0 if not tracking
 */
int TimeTrackingController::getCurrentElapsed() const
{
    if (!isTimerRunning()) {
        return 0;
    }
    
    return m_startTime.secsTo(QDateTime::currentDateTime());
}

/**
 * @brief Add a time entry to the model and database
 *
 * @param entry The TimeEntry object to add
 * @return true if the entry was added successfully, false otherwise
 */
bool TimeTrackingController::addTimeEntry(const TimeEntry& entry)
{
    // Add the entry to the model
    m_timeEntryModel->addTimeEntry(entry);
    
    // Save to the database
    bool success = DatabaseManager::instance().saveTimeEntry(entry);
    
    if (success) {
        emit timeEntryAdded(entry);
        qDebug() << "Added time entry:" << entry.id();
    } else {
        qWarning() << "Failed to add time entry to database";
    }
    
    return success;
}

/**
 * @brief Update an existing time entry
 *
 * @param entry The TimeEntry object with updated properties
 * @return true if the entry was updated successfully, false otherwise
 */
bool TimeTrackingController::updateTimeEntry(const TimeEntry& entry)
{
    // Update the entry in the model
    bool modelSuccess = m_timeEntryModel->updateTimeEntry(entry);
    
    if (!modelSuccess) {
        qWarning() << "Failed to update time entry in model:" << entry.id();
        return false;
    }
    
    // Save to the database
    bool dbSuccess = DatabaseManager::instance().saveTimeEntry(entry);
    
    if (dbSuccess) {
        emit timeEntryUpdated(entry);
        qDebug() << "Updated time entry:" << entry.id();
    } else {
        qWarning() << "Failed to update time entry in database";
    }
    
    return dbSuccess;
}

/**
 * @brief Delete a time entry
 *
 * @param id The ID of the time entry to delete
 * @return true if the entry was deleted successfully, false otherwise
 */
bool TimeTrackingController::deleteTimeEntry(const QString& id)
{
    // Get the entry before removing it (for the signal)
    TimeEntry entry = m_timeEntryModel->getTimeEntry(id);
    
    // Remove from the model
    bool modelSuccess = m_timeEntryModel->removeTimeEntry(id);
    
    if (!modelSuccess) {
        qWarning() << "Failed to remove time entry from model:" << id;
        return false;
    }
    
    // Delete from the database
    bool dbSuccess = DatabaseManager::instance().deleteTimeEntry(id);
    
    if (dbSuccess) {
        emit timeEntryDeleted(id);
        qDebug() << "Deleted time entry:" << id;
    } else {
        qWarning() << "Failed to delete time entry from database";
    }
    
    return dbSuccess;
}

/**
 * @brief Get time entries within a date range
 *
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 * @return A list of TimeEntry objects within the specified date range
 */
QList<TimeEntry> TimeTrackingController::getTimeEntries(const QDate& startDate, const QDate& endDate) const
{
    return m_timeEntryModel->getTimeEntriesInRange(startDate, endDate);
}

/**
 * @brief Get the total duration of time entries for today
 *
 * @return The total duration in seconds
 */
int TimeTrackingController::getTodayTotal() const
{
    return m_timeEntryModel->getTotalDuration(QDate::currentDate());
}

/**
 * @brief Get the total duration of time entries for the current week
 *
 * Calculates the date range for the current week (Monday to Sunday)
 * and sums the duration of all time entries in that range.
 *
 * @return The total duration in seconds
 */
int TimeTrackingController::getWeekTotal() const
{
    // Calculate the date range for the current week (Monday to Sunday)
    QDate today = QDate::currentDate();
    int dayOfWeek = today.dayOfWeek(); // 1 (Monday) to 7 (Sunday)
    
    QDate weekStart = today.addDays(-(dayOfWeek - 1));
    QDate weekEnd = weekStart.addDays(6);
    
    int total = 0;
    for (QDate date = weekStart; date <= weekEnd; date = date.addDays(1)) {
        total += m_timeEntryModel->getTotalDuration(date);
    }
    
    return total;
}

/**
 * @brief Get the most tracked project within a date range
 *
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 * @return The ID of the most tracked project, or empty string if none
 */
QString TimeTrackingController::getMostTrackedProject(const QDate& startDate, const QDate& endDate) const
{
    QMap<QString, int> projectDurations = getTimeByProject(startDate, endDate);
    
    QString mostTrackedProject;
    int maxDuration = 0;
    
    for (auto it = projectDurations.begin(); it != projectDurations.end(); ++it) {
        if (it.value() > maxDuration) {
            maxDuration = it.value();
            mostTrackedProject = it.key();
        }
    }
    
    return mostTrackedProject;
}

/**
 * @brief Get the duration of time spent on each project within a date range
 *
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 * @return A map of project IDs to durations in seconds
 */
QMap<QString, int> TimeTrackingController::getTimeByProject(const QDate& startDate, const QDate& endDate) const
{
    return m_timeEntryModel->getDurationsByProject(startDate, endDate);
}

/**
 * @brief Get the duration of time spent on each day within a date range
 *
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 * @return A map of dates to durations in seconds
 */
QMap<QDate, int> TimeTrackingController::getTimeByDay(const QDate& startDate, const QDate& endDate) const
{
    return m_timeEntryModel->getDurationsByDay(startDate, endDate);
}

/**
 * @brief Load time entries from the database
 *
 * @return true if entries were loaded successfully, false otherwise
 */
bool TimeTrackingController::loadTimeEntries()
{
    QList<TimeEntry> entries = DatabaseManager::instance().loadTimeEntries();
    m_timeEntryModel->setTimeEntries(entries);
    qDebug() << "Loaded" << entries.size() << "time entries from database";
    return true;
}

/**
 * @brief Save all time entries to the database
 *
 * @return true if entries were saved successfully, false otherwise
 */
bool TimeTrackingController::saveTimeEntries()
{
    QList<TimeEntry> entries = m_timeEntryModel->getTimeEntries();
    bool success = DatabaseManager::instance().saveTimeEntries(entries);
    
    if (success) {
        qDebug() << "Saved" << entries.size() << "time entries to database";
    } else {
        qWarning() << "Failed to save time entries to database";
    }
    
    return success;
}

/**
 * @brief Format a duration in seconds to a human-readable string
 *
 * @param seconds The duration in seconds
 * @param format The format string to use (default: "hh:mm:ss")
 *        - "h" = hours without leading zero
 *        - "hh" = hours with leading zero
 *        - "m" = minutes without leading zero
 *        - "mm" = minutes with leading zero
 *        - "s" = seconds without leading zero
 *        - "ss" = seconds with leading zero
 * @return The formatted duration string
 */
QString TimeTrackingController::formatDuration(int seconds, const QString& format)
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    QString result = format;
    result.replace("hh", QString("%1").arg(hours, 2, 10, QChar('0')));
    result.replace("h", QString::number(hours));
    result.replace("mm", QString("%1").arg(minutes, 2, 10, QChar('0')));
    result.replace("m", QString::number(minutes));
    result.replace("ss", QString("%1").arg(secs, 2, 10, QChar('0')));
    result.replace("s", QString::number(secs));
    
    return result;
}

/**
 * @brief Timer tick event handler
 *
 * Called every second when the timer is running. Emits the timerTick signal
 * with the current elapsed time.
 */
void TimeTrackingController::onTimerTick()
{
    int elapsed = getCurrentElapsed();
    emit timerTick(elapsed);
}
