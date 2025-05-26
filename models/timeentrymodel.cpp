/**
 * @file timeentrymodel.cpp
 * @brief Implementation of the TimeEntryModel class
 * 
 * The TimeEntryModel class manages a collection of TimeEntry objects and provides
 * an interface for the Qt Model/View framework. It inherits from QAbstractListModel
 * and implements the necessary methods for data access, modification, and role-based
 * data retrieval. It also provides methods for filtering and aggregating time entries.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "timeentrymodel.h"

/**
 * @brief Constructor for TimeEntryModel
 * 
 * @param parent The parent QObject
 */
TimeEntryModel::TimeEntryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

/**
 * @brief Destructor for TimeEntryModel
 */
TimeEntryModel::~TimeEntryModel()
{
}

/**
 * @brief Get the number of rows in the model
 * 
 * Implements QAbstractListModel::rowCount() to return the number of time entries.
 * 
 * @param parent The parent model index (unused in list models)
 * @return The number of time entries in the model
 */
int TimeEntryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_timeEntries.count();
}

/**
 * @brief Get data for a specific model index and role
 * 
 * Implements QAbstractListModel::data() to provide access to time entry data
 * based on the requested role.
 * 
 * @param index The model index to get data for
 * @param role The data role (DisplayRole, IdRole, ProjectIdRole, etc.)
 * @return The requested data as a QVariant
 */
QVariant TimeEntryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_timeEntries.size()) {
        return QVariant();
    }
    
    const TimeEntry &entry = m_timeEntries.at(index.row());
    
    switch (role) {
        case Qt::DisplayRole:
            return entry.startTime().toString("hh:mm") + " - " + 
                   (entry.isRunning() ? "Running" : QString::number(entry.duration() / 60) + " min");
        case IdRole:
            return entry.id();
        case ProjectIdRole:
            return entry.projectId();
        case StartTimeRole:
            return entry.startTime();
        case EndTimeRole:
            return entry.endTime();
        case DurationRole:
            return entry.duration();
        case NotesRole:
            return entry.notes();
        case IsRunningRole:
            return entry.isRunning();
        default:
            return QVariant();
    }
}

/**
 * @brief Set data for a specific model index and role
 * 
 * Implements QAbstractListModel::setData() to modify time entry properties
 * based on the specified role.
 * 
 * @param index The model index to set data for
 * @param value The new value to set
 * @param role The data role (ProjectIdRole, StartTimeRole, etc.)
 * @return true if the data was successfully set, false otherwise
 */
bool TimeEntryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_timeEntries.size()) {
        return false;
    }
    
    TimeEntry &entry = m_timeEntries[index.row()];
    
    bool success = false;
    
    switch (role) {
        case ProjectIdRole:
            entry.setProjectId(value.toString());
            success = true;
            break;
        case StartTimeRole:
            entry.setStartTime(value.toDateTime());
            success = true;
            break;
        case EndTimeRole:
            entry.setEndTime(value.toDateTime());
            success = true;
            break;
        case DurationRole:
            entry.setDuration(value.toInt());
            success = true;
            break;
        case NotesRole:
            entry.setNotes(value.toString());
            success = true;
            break;
        default:
            break;
    }
    
    if (success) {
        emit dataChanged(index, index, {role});
    }
    
    return success;
}

/**
 * @brief Get item flags for a model index
 * 
 * Implements QAbstractListModel::flags() to specify item capabilities.
 * 
 * @param index The model index to get flags for
 * @return The item flags (ItemIsEnabled, ItemIsSelectable, ItemIsEditable)
 */
Qt::ItemFlags TimeEntryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

/**
 * @brief Get the role names for the model
 * 
 * Implements QAbstractListModel::roleNames() to provide role names for QML integration.
 * 
 * @return A hash mapping role values to role names
 */
QHash<int, QByteArray> TimeEntryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[ProjectIdRole] = "projectId";
    roles[StartTimeRole] = "startTime";
    roles[EndTimeRole] = "endTime";
    roles[DurationRole] = "duration";
    roles[NotesRole] = "notes";
    roles[IsRunningRole] = "isRunning";
    return roles;
}

/**
 * @brief Add a time entry to the model
 * 
 * Adds a time entry to the beginning of the list (newest first) and notifies views of the change.
 * 
 * @param entry The TimeEntry object to add
 */
void TimeEntryModel::addTimeEntry(const TimeEntry &entry)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_timeEntries.prepend(entry);
    endInsertRows();
}

/**
 * @brief Remove a time entry from the model
 * 
 * Removes the time entry with the specified ID and notifies views of the change.
 * 
 * @param id The ID of the time entry to remove
 * @return true if the time entry was found and removed, false otherwise
 */
bool TimeEntryModel::removeTimeEntry(const QString &id)
{
    int index = findIndexById(id);
    if (index == -1) {
        return false;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    m_timeEntries.removeAt(index);
    endRemoveRows();
    
    return true;
}

/**
 * @brief Get a time entry by ID
 * 
 * @param id The ID of the time entry to retrieve
 * @return The TimeEntry object if found, or an empty TimeEntry otherwise
 */
TimeEntry TimeEntryModel::getTimeEntry(const QString &id) const
{
    int index = findIndexById(id);
    if (index == -1) {
        return TimeEntry();
    }
    
    return m_timeEntries.at(index);
}

/**
 * @brief Get all time entries
 * 
 * @return A list of all TimeEntry objects in the model
 */
QList<TimeEntry> TimeEntryModel::getTimeEntries() const
{
    return m_timeEntries;
}

/**
 * @brief Set the list of time entries
 * 
 * Replaces the entire list of time entries and notifies views of the change.
 * 
 * @param entries The new list of TimeEntry objects
 */
void TimeEntryModel::setTimeEntries(const QList<TimeEntry> &entries)
{
    beginResetModel();
    m_timeEntries = entries;
    endResetModel();
}

/**
 * @brief Update an existing time entry
 * 
 * Finds a time entry by ID and updates it with the provided TimeEntry object.
 * 
 * @param entry The TimeEntry object with updated properties
 * @return true if the time entry was found and updated, false otherwise
 */
bool TimeEntryModel::updateTimeEntry(const TimeEntry &entry)
{
    int index = findIndexById(entry.id());
    if (index == -1) {
        return false;
    }
    
    m_timeEntries[index] = entry;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
    
    return true;
}

/**
 * @brief Get time entries within a date range
 * 
 * Filters the time entries to include only those that start within the specified date range.
 * 
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 * @return A list of TimeEntry objects within the specified date range
 */
QList<TimeEntry> TimeEntryModel::getTimeEntriesInRange(const QDate &startDate, const QDate &endDate) const
{
    QList<TimeEntry> result;
    
    // Make the end date inclusive by setting it to the end of the day
    QDateTime rangeStart = QDateTime(startDate, QTime(0, 0, 0));
    QDateTime rangeEnd = QDateTime(endDate, QTime(23, 59, 59, 999));
    
    for (const TimeEntry &entry : m_timeEntries) {
        // Check if the entry's start time falls within the range
        if (entry.startTime() >= rangeStart && entry.startTime() <= rangeEnd) {
            result.append(entry);
        }
    }
    
    return result;
}

/**
 * @brief Get time entries for a specific project
 * 
 * Filters the time entries to include only those associated with the specified project.
 * 
 * @param projectId The ID of the project
 * @return A list of TimeEntry objects for the specified project
 */
QList<TimeEntry> TimeEntryModel::getTimeEntriesForProject(const QString &projectId) const
{
    QList<TimeEntry> result;
    
    for (const TimeEntry &entry : m_timeEntries) {
        if (entry.projectId() == projectId) {
            result.append(entry);
        }
    }
    
    return result;
}

/**
 * @brief Get the total duration of time entries for a specific date
 * 
 * Calculates the sum of durations for all time entries that occur
 * wholly or partially on the specified date.
 * 
 * @param date The date to calculate the total for
 * @return The total duration in seconds
 */
int TimeEntryModel::getTotalDuration(const QDate &date) const
{
    int total = 0;
    
    QDateTime dayStart = QDateTime(date, QTime(0, 0, 0));
    QDateTime dayEnd = QDateTime(date, QTime(23, 59, 59, 999));
    
    for (const TimeEntry &entry : m_timeEntries) {
        // Only count entries that start or end within the specified date
        if ((entry.startTime() >= dayStart && entry.startTime() <= dayEnd) ||
            (!entry.endTime().isNull() && entry.endTime() >= dayStart && entry.endTime() <= dayEnd)) {
                
            // If the entry spans beyond the day, only count the portion within the day
            QDateTime entryStart = entry.startTime() < dayStart ? dayStart : entry.startTime();
            QDateTime entryEnd = entry.isRunning() ? 
                QDateTime::currentDateTime() : 
                (entry.endTime() > dayEnd ? dayEnd : entry.endTime());
                
            total += entryStart.secsTo(entryEnd);
        }
    }
    
    return total;
}

/**
 * @brief Get the duration of time spent on each project within a date range
 * 
 * Calculates the sum of durations for each project within the specified date range.
 * 
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 * @return A map of project IDs to durations in seconds
 */
QMap<QString, int> TimeEntryModel::getDurationsByProject(const QDate &startDate, const QDate &endDate) const
{
    QMap<QString, int> result;
    
    QDateTime rangeStart = QDateTime(startDate, QTime(0, 0, 0));
    QDateTime rangeEnd = QDateTime(endDate, QTime(23, 59, 59, 999));
    
    for (const TimeEntry &entry : m_timeEntries) {
        // Only count entries that start or end within the date range
        if ((entry.startTime() >= rangeStart && entry.startTime() <= rangeEnd) ||
            (!entry.endTime().isNull() && entry.endTime() >= rangeStart && entry.endTime() <= rangeEnd)) {
                
            // If the entry spans beyond the range, only count the portion within the range
            QDateTime entryStart = entry.startTime() < rangeStart ? rangeStart : entry.startTime();
            QDateTime entryEnd = entry.isRunning() ? 
                QDateTime::currentDateTime() : 
                (entry.endTime() > rangeEnd ? rangeEnd : entry.endTime());
                
            int duration = entryStart.secsTo(entryEnd);
            
            // Add to the running total for this project
            result[entry.projectId()] = result.value(entry.projectId(), 0) + duration;
        }
    }
    
    return result;
}

/**
 * @brief Get the duration of time spent on each day within a date range
 * 
 * Calculates the sum of durations for each day within the specified date range.
 * 
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 * @return A map of dates to durations in seconds
 */
QMap<QDate, int> TimeEntryModel::getDurationsByDay(const QDate &startDate, const QDate &endDate) const
{
    QMap<QDate, int> result;
    
    // Initialize the map with zero values for all days in the range
    for (QDate date = startDate; date <= endDate; date = date.addDays(1)) {
        result[date] = 0;
    }
    
    // Calculate the total for each day
    for (QDate date = startDate; date <= endDate; date = date.addDays(1)) {
        result[date] = getTotalDuration(date);
    }
    
    return result;
}

/**
 * @brief Get the currently running time entry
 * 
 * Finds the first time entry that is in a running state (no end time).
 * 
 * @return The running TimeEntry object, or an empty TimeEntry if none is running
 */
TimeEntry TimeEntryModel::getRunningTimeEntry() const
{
    for (const TimeEntry &entry : m_timeEntries) {
        if (entry.isRunning()) {
            return entry;
        }
    }
    
    return TimeEntry();
}

/**
 * @brief Check if there is a currently running time entry
 * 
 * @return true if there is a running time entry, false otherwise
 */
bool TimeEntryModel::hasRunningTimeEntry() const
{
    for (const TimeEntry &entry : m_timeEntries) {
        if (entry.isRunning()) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Find the index of a time entry by ID
 * 
 * Helper method to find the index of a time entry in the list by its ID.
 * 
 * @param id The ID of the time entry to find
 * @return The index of the time entry, or -1 if not found
 */
int TimeEntryModel::findIndexById(const QString &id) const
{
    for (int i = 0; i < m_timeEntries.size(); ++i) {
        if (m_timeEntries.at(i).id() == id) {
            return i;
        }
    }
    
    return -1;
}
