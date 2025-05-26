/**
 * @file timeentrymodel.h
 * @brief Definition of the TimeEntryModel class
 * 
 * This file defines the TimeEntryModel class which serves as a data model
 * for collections of time entries in the application. It inherits from QAbstractListModel
 * to provide model/view integration with Qt widgets.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QDate>
#include <QMap>
#include "timeentry.h"

/**
 * @class TimeEntryModel
 * @brief Model for time entries
 * 
 * The TimeEntryModel class provides a model for working with collections of TimeEntry objects.
 * It implements the necessary methods from QAbstractListModel to support the Qt model/view architecture.
 * Additionally, it provides methods for filtering, aggregating, and manipulating time entries.
 */
class TimeEntryModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    /**
     * @brief Custom roles for time entry data
     */
    enum TimeEntryRoles {
        IdRole = Qt::UserRole + 1,
        ProjectIdRole,
        StartTimeRole,
        EndTimeRole,
        DurationRole,
        NotesRole,
        IsRunningRole
    };
    
    /**
     * @brief Constructor
     * 
     * Creates a new time entry model.
     * 
     * @param parent Optional parent object
     */
    explicit TimeEntryModel(QObject *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~TimeEntryModel();

    // QAbstractItemModel implementation
    
    /**
     * @brief Get the number of rows in the model
     * @param parent Parent model index (unused)
     * @return int Number of time entries in the model
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    
    /**
     * @brief Get data for a specific index and role
     * @param index Model index to get data for
     * @param role Data role
     * @return QVariant The requested data
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    /**
     * @brief Set data for a specific index and role
     * @param index Model index to set data for
     * @param value The value to set
     * @param role Data role
     * @return bool True if data was set successfully, false otherwise
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    
    /**
     * @brief Get the item flags for a specific index
     * @param index Model index to get flags for
     * @return Qt::ItemFlags The item flags
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    /**
     * @brief Get the role names for use with QML
     * @return QHash<int, QByteArray> The role names
     */
    QHash<int, QByteArray> roleNames() const override;

    // Time entry management methods
    
    /**
     * @brief Add a time entry to the model
     * @param entry The time entry to add
     */
    void addTimeEntry(const TimeEntry &entry);
    
    /**
     * @brief Remove a time entry from the model
     * @param id The ID of the entry to remove
     * @return bool True if the entry was removed, false otherwise
     */
    bool removeTimeEntry(const QString &id);
    
    /**
     * @brief Get a time entry by ID
     * @param id The ID of the entry to get
     * @return TimeEntry The time entry with the specified ID, or an empty entry if not found
     */
    TimeEntry getTimeEntry(const QString &id) const;
    
    /**
     * @brief Get all time entries
     * @return QList<TimeEntry> All time entries in the model
     */
    QList<TimeEntry> getTimeEntries() const;
    
    /**
     * @brief Set all time entries
     * @param entries The time entries to set
     */
    void setTimeEntries(const QList<TimeEntry> &entries);
    
    /**
     * @brief Update a time entry
     * @param entry The time entry to update
     * @return bool True if the entry was updated, false otherwise
     */
    bool updateTimeEntry(const TimeEntry &entry);

    // Filtering and aggregation methods
    
    /**
     * @brief Get the time entries for a specific date range
     * @param startDate Start date of the range
     * @param endDate End date of the range
     * @return QList<TimeEntry> Time entries in the date range
     */
    QList<TimeEntry> getTimeEntriesInRange(const QDate &startDate, const QDate &endDate) const;
    
    /**
     * @brief Get the time entries for a specific project
     * @param projectId The ID of the project
     * @return QList<TimeEntry> Time entries for the specified project
     */
    QList<TimeEntry> getTimeEntriesForProject(const QString &projectId) const;
    
    
    /**
     * @brief Get the total duration for a specific date
     * @param date The date to get duration for
     * @return int Total duration in seconds
     */
    int getTotalDuration(const QDate &date) const;
    
    
    /**
     * @brief Get total duration by project for a date range
     * @param startDate Start date of the range
     * @param endDate End date of the range
     * @return QMap<QString, int> Map of project IDs to total durations in seconds
     */
    QMap<QString, int> getDurationsByProject(const QDate &startDate, const QDate &endDate) const;
    
    /**
     * @brief Get the total durations by day for a date range
     * @param startDate Start date of the range
     * @param endDate End date of the range
     * @return QMap<QDate, int> Map of dates to total durations in seconds
     */
    QMap<QDate, int> getDurationsByDay(const QDate &startDate, const QDate &endDate) const;
    
    /**
     * @brief Get any currently running time entry
     * @return TimeEntry The currently running time entry, or an empty entry if none is running
     */
    TimeEntry getRunningTimeEntry() const;
    
    /**
     * @brief Check if there is a running time entry
     * @return bool True if there is a running time entry, false otherwise
     */
    bool hasRunningTimeEntry() const;

private:
    QList<TimeEntry> m_timeEntries;  ///< The list of time entries
    
    /**
     * @brief Find the index of an entry with the specified ID
     * @param id The ID to search for
     * @return int The index of the entry, or -1 if not found
     */
    int findIndexById(const QString &id) const;
};
