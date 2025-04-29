/**
 * @file taskmodel.h
 * @brief Definition of the TaskModel class
 * 
 * This file defines the TaskModel class which manages a collection of Task objects
 * and implements the Qt Model/View architecture for displaying tasks in views.
 * It provides methods for task management, filtering, sorting, and drag-and-drop support.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QAbstractListModel>
#include "task.h"

/**
 * @class TaskModel
 * @brief Model for managing a collection of tasks
 * 
 * The TaskModel class implements QAbstractListModel to provide a model
 * for task data that can be used with Qt's Model/View architecture.
 * It manages a collection of Task objects, provides methods for adding,
 * removing, and modifying tasks, and supports filtering, sorting, and
 * drag-and-drop reordering.
 */
class TaskModel : public QAbstractListModel {
    Q_OBJECT

public:
    /**
     * @brief Custom roles for accessing task data
     * 
     * These roles extend Qt's standard model roles to provide
     * access to specific task properties in views and delegates.
     */
    enum TaskRoles {
        IdRole = Qt::UserRole + 1,     ///< Role for accessing the task ID
        TitleRole,                     ///< Role for accessing the task title
        DescriptionRole,               ///< Role for accessing the task description
        CompletedRole,                 ///< Role for accessing the task completion status
        CreatedDateRole,               ///< Role for accessing the task creation date
        DueDateRole,                   ///< Role for accessing the task due date
        CategoryIdRole,                ///< Role for accessing the task category ID
        PriorityRole,                  ///< Role for accessing the task priority
        DisplayOrderRole               ///< Role for accessing the task display order
    };

    /**
     * @brief Constructor
     * 
     * Creates a new empty TaskModel.
     * 
     * @param parent Optional parent QObject
     */
    explicit TaskModel(QObject *parent = nullptr);

    // QAbstractItemModel implementation
    /**
     * @brief Get the number of rows (tasks) in the model
     * 
     * @param parent Parent model index (unused in list models)
     * @return int Number of tasks in the model
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    
    /**
     * @brief Get data for a specific item and role
     * 
     * @param index Model index identifying the item
     * @param role Data role to retrieve
     * @return QVariant The requested data
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    /**
     * @brief Set data for a specific item and role
     * 
     * @param index Model index identifying the item
     * @param value New value to set
     * @param role Data role to modify
     * @return bool True if data was successfully set
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    
    /**
     * @brief Get item flags for a specific index
     * 
     * @param index Model index to get flags for
     * @return Qt::ItemFlags Flags for the specified item
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    /**
     * @brief Get role names used for QML integration
     * 
     * @return QHash<int, QByteArray> Mapping from role values to role names
     */
    QHash<int, QByteArray> roleNames() const override;

    // Drag and drop support
    /**
     * @brief Get supported drop actions
     * 
     * @return Qt::DropActions Supported drop actions (Move)
     */
    Qt::DropActions supportedDropActions() const override;
    
    /**
     * @brief Check if data can be dropped at the specified position
     * 
     * @param data MIME data being dropped
     * @param action Drop action being performed
     * @param row Target row
     * @param column Target column
     * @param parent Parent model index
     * @return bool True if the data can be dropped here
     */
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    
    /**
     * @brief Handle dropping data at the specified position
     * 
     * @param data MIME data being dropped
     * @param action Drop action being performed
     * @param row Target row
     * @param column Target column
     * @param parent Parent model index
     * @return bool True if the drop was successful
     */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    
    /**
     * @brief Create MIME data for dragging items
     * 
     * @param indexes List of indexes being dragged
     * @return QMimeData* MIME data representing the dragged items
     */
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    
    /**
     * @brief Get supported MIME types for drag and drop
     * 
     * @return QStringList List of supported MIME types
     */
    QStringList mimeTypes() const override;

    // Task management methods
    /**
     * @brief Add a task to the model
     * 
     * @param task The task to add
     */
    void addTask(const Task &task);
    
    /**
     * @brief Remove a task from the model
     * 
     * @param id ID of the task to remove
     * @return bool True if the task was found and removed
     */
    bool removeTask(const QString &id);
    
    /**
     * @brief Get a task by ID
     * 
     * @param id ID of the task to retrieve
     * @return Task The requested task
     */
    Task getTask(const QString &id) const;
    
    /**
     * @brief Get all tasks in the model
     * 
     * @return QList<Task> List of all tasks
     */
    QList<Task> getTasks() const;
    
    /**
     * @brief Replace all tasks in the model
     * 
     * @param tasks New list of tasks
     */
    void setTasks(const QList<Task> &tasks);

    // Task reordering methods
    /**
     * @brief Move a task from one position to another
     * 
     * @param fromRow Source row
     * @param toRow Destination row
     * @return bool True if the move was successful
     */
    bool moveTask(int fromRow, int toRow);
    
    /**
     * @brief Update display orders for all tasks
     * 
     * Ensures all tasks have sequential display orders.
     */
    void updateDisplayOrders();

    // Filtering and sorting
    /**
     * @brief Filter tasks by category
     * 
     * Shows only tasks belonging to the specified category.
     * 
     * @param categoryId ID of the category to filter by
     */
    void filterByCategory(const QString &categoryId);
    
    /**
     * @brief Clear any active filters
     * 
     * Shows all tasks regardless of category.
     */
    void clearFilter();
    
    /**
     * @brief Sort tasks by due date
     * 
     * @param ascending If true, sorts from earliest to latest due date
     */
    void sortByDueDate(bool ascending = true);
    
    /**
     * @brief Sort tasks by priority
     * 
     * @param ascending If true, sorts from lowest to highest priority
     */
    void sortByPriority(bool ascending = false);

private:
    QList<Task> m_tasks;          ///< All tasks in the model
    QList<Task> m_filteredTasks;  ///< Tasks after filtering
    bool m_isFiltered;            ///< Flag indicating if filtering is active

    /**
     * @brief Get the next available display order
     * 
     * @return int The next available display order value
     */
    int getNextDisplayOrder() const;
};
