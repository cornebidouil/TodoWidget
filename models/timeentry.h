/**
 * @file timeentry.h
 * @brief Definition of the TimeEntry model class
 * 
 * This file defines the TimeEntry class which represents a single time tracking
 * record in the application. It contains all the properties of a time entry
 * including its identifier, associated task, start and end times, duration, and notes.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @class TimeEntry
 * @brief Represents a single time tracking record
 * 
 * The TimeEntry class encapsulates all data related to a time tracking record.
 * It provides methods for accessing and modifying time entry properties, as well as
 * serialization to/from JSON for storage and interchange.
 */
class TimeEntry {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty time entry with default values.
     */
    TimeEntry();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a new time entry with the specified project ID and start time.
     * 
     * @param projectId The ID of the associated project
     * @param startTime The start time of the time entry
     */
    TimeEntry(const QString& projectId, const QDateTime& startTime);
    
    /**
     * @brief Complete parameterized constructor
     * 
     * Creates a new time entry with all properties specified.
     * 
     * @param projectId The ID of the associated project
     * @param startTime The start time of the time entry
     * @param endTime The end time of the time entry
     * @param duration The duration in seconds (optional, calculated from times if not provided)
     * @param notes Optional notes for the time entry
     */
    TimeEntry(const QString& projectId, const QDateTime& startTime, const QDateTime& endTime, 
              int duration = -1, const QString& notes = QString());

    /**
     * @brief Get the time entry's unique identifier
     * @return QString The time entry ID
     */
    QString id() const { return m_id; }
    
    /**
     * @brief Get the associated project's ID
     * @return QString The project ID
     */
    QString projectId() const { return m_projectId; }
    
    /**
     * @brief Get the start time
     * @return QDateTime The time when tracking started
     */
    QDateTime startTime() const { return m_startTime; }
    
    /**
     * @brief Get the end time
     * @return QDateTime The time when tracking ended (can be null)
     */
    QDateTime endTime() const { return m_endTime; }
    
    /**
     * @brief Get the duration in seconds
     * @return int The duration in seconds
     */
    int duration() const;
    
    /**
     * @brief Get the notes
     * @return QString The notes associated with this time entry
     */
    QString notes() const { return m_notes; }

    /**
     * @brief Set the time entry's unique identifier
     * @param id The unique identifier to set
     */
    void setId(const QString& id) { m_id = id; }
    
    /**
     * @brief Set the associated project
     * @param projectId The ID of the project
     */
    void setProjectId(const QString& projectId) { m_projectId = projectId; }
    
    /**
     * @brief Set the start time
     * @param startTime The start time to set
     */
    void setStartTime(const QDateTime& startTime) { m_startTime = startTime; }
    
    /**
     * @brief Set the end time
     * @param endTime The end time to set
     */
    void setEndTime(const QDateTime& endTime) { m_endTime = endTime; }
    
    /**
     * @brief Set the duration manually
     * @param duration The duration in seconds
     */
    void setDuration(int duration) { m_duration = duration; }
    
    /**
     * @brief Set the notes
     * @param notes The notes to set
     */
    void setNotes(const QString& notes) { m_notes = notes; }
    
    /**
     * @brief Check if the time entry is currently running
     * @return bool True if the time entry is still running (no end time), false otherwise
     */
    bool isRunning() const;
    
    /**
     * @brief Stop a running time entry by setting the end time to the current time
     * @return bool True if the entry was running and stopped, false if it was already stopped
     */
    bool stop();
    
    /**
     * @brief Calculate the elapsed time for a running entry
     * @return int The elapsed time in seconds from start time to now
     */
    int elapsedSeconds() const;

    /**
     * @brief Convert the time entry to a JSON object
     * 
     * Serializes all time entry properties to a JSON object for storage or transmission.
     * 
     * @return QJsonObject The JSON representation of the time entry
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Create a time entry from a JSON object
     * 
     * Static factory method to deserialize a time entry from its JSON representation.
     * 
     * @param json The JSON object containing time entry data
     * @return TimeEntry A new TimeEntry instance with properties from the JSON object
     */
    static TimeEntry fromJson(const QJsonObject& json);

private:
    QString m_id;             ///< Unique identifier
    QString m_projectId;      ///< Associated project ID
    QDateTime m_startTime;    ///< When tracking started
    QDateTime m_endTime;      ///< When tracking ended (can be null)
    int m_duration;           ///< Duration in seconds (calculated or manual)
    QString m_notes;          ///< Optional notes
};
