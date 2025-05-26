/**
 * @file timeentry.cpp
 * @brief Implementation of the TimeEntry model class
 * 
 * The TimeEntry class represents a period of time spent on a project.
 * It contains information about the start and end times, duration, 
 * associated project, and optional notes. TimeEntry objects can be
 * in a "running" state (no end time) or completed state.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "timeentry.h"
#include <QUuid>

/**
 * @brief Default constructor for TimeEntry
 * 
 * Creates a new TimeEntry with a generated UUID, current timestamp as the
 * start time, and no end time or duration (running state).
 */
TimeEntry::TimeEntry()
    : m_id(QUuid::createUuid().toString().remove('{').remove('}'))
    , m_projectId("")
    , m_startTime(QDateTime::currentDateTime())
    , m_endTime(QDateTime())
    , m_duration(-1)
    , m_notes("")
{
}

/**
 * @brief Constructor with project ID and start time
 * 
 * Creates a new TimeEntry with a generated UUID, specified project ID and
 * start time, and no end time or duration (running state).
 * 
 * @param projectId The ID of the associated project
 * @param startTime The start time of the time entry
 */
TimeEntry::TimeEntry(const QString& projectId, const QDateTime& startTime)
    : m_id(QUuid::createUuid().toString().remove('{').remove('}'))
    , m_projectId(projectId)
    , m_startTime(startTime)
    , m_endTime(QDateTime())
    , m_duration(-1)
    , m_notes("")
{
}

/**
 * @brief Constructor with all properties
 * 
 * Creates a new TimeEntry with a generated UUID and all properties specified.
 * 
 * @param projectId The ID of the associated project
 * @param startTime The start time of the time entry
 * @param endTime The end time of the time entry
 * @param duration The duration in seconds (optional, can be calculated)
 * @param notes Optional notes about the time entry
 */
TimeEntry::TimeEntry(const QString& projectId, const QDateTime& startTime, const QDateTime& endTime, 
                     int duration, const QString& notes)
    : m_id(QUuid::createUuid().toString().remove('{').remove('}'))
    , m_projectId(projectId)
    , m_startTime(startTime)
    , m_endTime(endTime)
    , m_duration(duration)
    , m_notes(notes)
{
}

/**
 * @brief Get the duration of the time entry
 * 
 * Returns the duration in seconds. If the duration was manually set,
 * returns that value. Otherwise, calculates the duration based on
 * start and end times, or from start time to current time if still running.
 * 
 * @return The duration in seconds
 */
int TimeEntry::duration() const
{
    // If duration was manually set, return that value
    if (m_duration >= 0) {
        return m_duration;
    }
    
    // If entry is complete (has end time), calculate from start to end
    if (!m_endTime.isNull()) {
        return m_startTime.secsTo(m_endTime);
    }
    
    // Otherwise, calculate from start time to now
    return elapsedSeconds();
}

/**
 * @brief Check if the time entry is currently running
 * 
 * A time entry is considered running if it has no end time.
 * 
 * @return true if the time entry is running, false otherwise
 */
bool TimeEntry::isRunning() const
{
    return m_endTime.isNull();
}

/**
 * @brief Stop a running time entry
 * 
 * Sets the end time to the current time and calculates the duration.
 * Has no effect if the time entry is already stopped.
 * 
 * @return true if the time entry was running and was stopped, false otherwise
 */
bool TimeEntry::stop()
{
    if (isRunning()) {
        m_endTime = QDateTime::currentDateTime();
        // Calculate and store the duration
        m_duration = m_startTime.secsTo(m_endTime);
        return true;
    }
    return false;
}

/**
 * @brief Get the elapsed time for a running time entry
 * 
 * Calculates the elapsed time from the start time to the current time
 * for a running time entry.
 * 
 * @return The elapsed time in seconds, or 0 if not running
 */
int TimeEntry::elapsedSeconds() const
{
    if (isRunning()) {
        return m_startTime.secsTo(QDateTime::currentDateTime());
    }
    return 0;
}

/**
 * @brief Convert the TimeEntry to a JSON object
 * 
 * Serializes all time entry properties to a JSON object for storage.
 * 
 * @return QJsonObject containing the time entry data
 */
QJsonObject TimeEntry::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["projectId"] = m_projectId;
    json["startTime"] = m_startTime.toString(Qt::ISODate);
    
    if (!m_endTime.isNull()) {
        json["endTime"] = m_endTime.toString(Qt::ISODate);
    }
    
    if (m_duration >= 0) {
        json["duration"] = m_duration;
    }
    
    if (!m_notes.isEmpty()) {
        json["notes"] = m_notes;
    }
    
    return json;
}

/**
 * @brief Create a TimeEntry from a JSON object
 * 
 * Static factory method that deserializes a TimeEntry from a JSON object.
 * 
 * @param json The JSON object containing time entry data
 * @return A new TimeEntry instance with properties from the JSON
 */
TimeEntry TimeEntry::fromJson(const QJsonObject& json)
{
    TimeEntry entry;
    
    if (json.contains("id")) {
        entry.setId(json["id"].toString());
    }
    
    if (json.contains("projectId")) {
        entry.setProjectId(json["projectId"].toString());
    }
    
    if (json.contains("startTime")) {
        entry.setStartTime(QDateTime::fromString(json["startTime"].toString(), Qt::ISODate));
    }
    
    if (json.contains("endTime")) {
        entry.setEndTime(QDateTime::fromString(json["endTime"].toString(), Qt::ISODate));
    }
    
    if (json.contains("duration")) {
        entry.setDuration(json["duration"].toInt());
    }
    
    if (json.contains("notes")) {
        entry.setNotes(json["notes"].toString());
    }
    
    return entry;
}
