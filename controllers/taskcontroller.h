/**
 * @file taskcontroller.h
 * @brief Definition of the TaskController class
 * 
 * This file defines the TaskController class which manages operations on Task objects
 * and coordinates between the TaskModel and database. It handles CRUD operations,
 * filtering, sorting, and persistence of tasks.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QObject>
#include "../models/task.h"
#include "../models/taskmodel.h"
#include "../services/databasemanager.h"

/**
 * @class TaskController
 * @brief Controller for task-related operations
 * 
 * The TaskController class implements the business logic for managing tasks.
 * It acts as an intermediary between the view and the model/database,
 * handling operations such as adding, updating, and deleting tasks,
 * as well as filtering, sorting, and persistence operations.
 * 
 * It is implemented as a singleton to ensure there is only one instance
 * throughout the application.
 */
class TaskController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     * @param model Optional task model to use (only used on first call)
     * @return TaskController& Reference to the singleton instance
     */
    static TaskController& instance(TaskModel* model = nullptr);

    /**
     * @brief Destructor
     */
    ~TaskController();

    /**
     * @brief Cleanup the singleton instance
     * 
     * Deletes the singleton instance and sets it to nullptr.
     * Useful for testing and application shutdown.
     */
    static void cleanup();

    /**
     * @brief Get the task model being managed
     * @return TaskModel* Pointer to the task model
     */
    TaskModel* model() const { return m_taskModel; }

    /**
     * @brief Add a new task
     * 
     * Creates and adds a new task with the specified properties.
     * 
     * @param title The title of the task
     * @param categoryId The ID of the category the task belongs to
     * @param description Optional description of the task
     * @param dueDate Optional due date for the task
     * @param priority Optional priority level (1-5, default: 3)
     * @return bool True if the task was successfully added, false otherwise
     */
    bool addTask(const QString& title, const QString& categoryId, const QString& description = QString(),
                const QDateTime& dueDate = QDateTime(), int priority = 3);
    
    /**
     * @brief Update an existing task
     * 
     * Updates the properties of an existing task identified by its ID.
     * 
     * @param id The ID of the task to update
     * @param title The new title
     * @param categoryId The new category ID
     * @param description The new description
     * @param dueDate The new due date
     * @param priority The new priority level
     * @return bool True if the task was successfully updated, false otherwise
     */
    bool updateTask(const QString& id, const QString& title, const QString& categoryId,
                   const QString& description, const QDateTime& dueDate, int priority);
    
    /**
     * @brief Toggle the completion status of a task
     * 
     * Flips the completed status of a task identified by its ID.
     * 
     * @param id The ID of the task to toggle
     * @return bool True if the task was successfully updated, false otherwise
     */
    bool toggleTaskCompletion(const QString& id);
    
    /**
     * @brief Delete a task
     * 
     * Removes a task identified by its ID from the model and database.
     * 
     * @param id The ID of the task to delete
     * @return bool True if the task was successfully deleted, false otherwise
     */
    bool deleteTask(const QString& id);
    
    /**
     * @brief Delete all completed tasks
     * 
     * Removes all tasks that have been marked as completed.
     * 
     * @return bool True if the operation was successful, false otherwise
     */
    bool deleteCompletedTasks();
    
    /**
     * @brief Move a task to a new position
     * 
     * Reorders tasks by moving a task from one position to another.
     * 
     * @param fromRow The current row index of the task
     * @param toRow The target row index for the task
     * @return bool True if the task was successfully moved, false otherwise
     */
    bool moveTask(int fromRow, int toRow);

    /**
     * @brief Filter tasks by category
     * 
     * Shows only tasks that belong to the specified category.
     * 
     * @param categoryId The ID of the category to filter by
     */
    void filterByCategory(const QString& categoryId);
    
    /**
     * @brief Clear any active filters
     * 
     * Resets all filters to show all tasks.
     */
    void clearFilter();
    
    /**
     * @brief Sort tasks by due date
     * 
     * @param ascending If true, sorts from earliest to latest; if false, from latest to earliest
     */
    void sortByDueDate(bool ascending = true);
    
    /**
     * @brief Sort tasks by priority
     * 
     * @param ascending If true, sorts from lowest to highest; if false, from highest to lowest
     */
    void sortByPriority(bool ascending = false);

    /**
     * @brief Load tasks from the database
     * 
     * Retrieves all tasks from the database and populates the model.
     * 
     * @return bool True if tasks were successfully loaded, false otherwise
     */
    bool loadTasks();
    
    /**
     * @brief Save tasks to the database
     * 
     * Persists all tasks in the model to the database.
     * 
     * @return bool True if tasks were successfully saved, false otherwise
     */
    bool saveTasks();

signals:
    /**
     * @brief Signal emitted when tasks have been changed
     * 
     * This signal is emitted whenever tasks are added, updated, deleted, 
     * or their order changes, allowing views to update accordingly.
     */
    void tasksChanged();

private:
    /**
     * @brief Private constructor to enforce singleton pattern
     * @param model The task model to manage
     * @param parent Optional parent QObject
     */
    explicit TaskController(TaskModel* model, QObject* parent = nullptr);

    /**
     * @brief Private copy constructor to enforce singleton pattern
     */
    TaskController(const TaskController&) = delete;
    
    /**
     * @brief Private assignment operator to enforce singleton pattern
     */
    TaskController& operator=(const TaskController&) = delete;

    TaskModel* m_taskModel;  ///< Pointer to the task model being managed
    static TaskController* s_instance;  ///< Singleton instance
};
