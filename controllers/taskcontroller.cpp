/**
 * @file taskcontroller.cpp
 * @brief Implementation of the TaskController class
 * 
 * This file implements the TaskController class which manages operations on Task objects
 * and coordinates between the TaskModel and database. It handles CRUD operations,
 * filtering, sorting, and persistence of tasks.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "taskcontroller.h"
#include <QDebug>

/**
 * @brief Constructor
 * 
 * Creates a new TaskController connected to the specified TaskModel.
 * 
 * @param model Pointer to the TaskModel to be managed
 * @param parent Optional parent QObject
 */
TaskController::TaskController(TaskModel* model, QObject* parent)
    : QObject(parent), m_taskModel(model)
{
}

/**
 * @brief Add a new task
 * 
 * Creates and adds a new task with the specified properties.
 * The task is added to both the model and the database.
 * 
 * @param title The title of the task
 * @param categoryId The ID of the category the task belongs to
 * @param description Optional description of the task
 * @param dueDate Optional due date for the task
 * @param priority Optional priority level (1-5, default: 3)
 * @return bool True if the task was successfully added, false otherwise
 */
bool TaskController::addTask(const QString& title, const QString& categoryId, const QString& description,
                           const QDateTime& dueDate, int priority)
{
    // Validate required fields
    if (title.isEmpty()) {
        return false;
    }

    // Create the task with the specified properties
    Task task(title, categoryId);
    task.setDescription(description);

    // Only set due date if it's valid
    if (dueDate.isValid()) {
        task.setDueDate(dueDate);
    }

    task.setPriority(priority);

    // Add to model
    m_taskModel->addTask(task);

    // Save to database
    if (!DatabaseManager::instance().saveTask(task)) {
        return false;
    }

    // Notify listeners about the change
    emit tasksChanged();
    return true;
}

/**
 * @brief Update an existing task
 * 
 * Updates the properties of an existing task identified by its ID.
 * The changes are applied to both the model and the database.
 * 
 * @param id The ID of the task to update
 * @param title The new title
 * @param categoryId The new category ID
 * @param description The new description
 * @param dueDate The new due date
 * @param priority The new priority level
 * @return bool True if the task was successfully updated, false otherwise
 */
bool TaskController::updateTask(const QString& id, const QString& title, const QString& categoryId,
                              const QString& description, const QDateTime& dueDate, int priority)
{
    // Validate required fields
    if (title.isEmpty()) {
        return false;
    }

    // Get the existing task
    Task task = m_taskModel->getTask(id);
    if (task.id().isEmpty()) {
        return false;  // Task not found
    }

    // Update task properties
    task.setTitle(title);
    task.setCategoryId(categoryId);
    task.setDescription(description);
    task.setDueDate(dueDate);
    task.setPriority(priority);

    // Find and update the task in the model
    for (int i = 0; i < m_taskModel->rowCount(); ++i) {
        QModelIndex index = m_taskModel->index(i, 0);
        if (m_taskModel->data(index, TaskModel::IdRole).toString() == id) {
            m_taskModel->setData(index, title, TaskModel::TitleRole);
            m_taskModel->setData(index, description, TaskModel::DescriptionRole);
            m_taskModel->setData(index, categoryId, TaskModel::CategoryIdRole);
            m_taskModel->setData(index, dueDate, TaskModel::DueDateRole);
            m_taskModel->setData(index, priority, TaskModel::PriorityRole);
            break;
        }
    }

    // Save to database
    if (!DatabaseManager::instance().saveTask(task)) {
        return false;
    }

    // Notify listeners about the change
    emit tasksChanged();
    return true;
}

/**
 * @brief Toggle the completion status of a task
 * 
 * Flips the completed status of a task identified by its ID.
 * The change is applied to both the model and the database.
 * 
 * @param id The ID of the task to toggle
 * @return bool True if the task was successfully updated, false otherwise
 */
bool TaskController::toggleTaskCompletion(const QString& id)
{
    // Get the existing task
    Task task = m_taskModel->getTask(id);
    if (task.id().isEmpty()) {
        return false;  // Task not found
    }

    // Toggle completion status
    task.setCompleted(!task.isCompleted());

    // Find and update the task in the model
    for (int i = 0; i < m_taskModel->rowCount(); ++i) {
        QModelIndex index = m_taskModel->index(i, 0);
        if (m_taskModel->data(index, TaskModel::IdRole).toString() == id) {
            m_taskModel->setData(index, task.isCompleted(), TaskModel::CompletedRole);
            break;
        }
    }

    // Save to database
    if (!DatabaseManager::instance().saveTask(task)) {
        return false;
    }

    // Notify listeners about the change
    emit tasksChanged();
    return true;
}

/**
 * @brief Delete a task
 * 
 * Removes a task identified by its ID from the model and database.
 * 
 * @param id The ID of the task to delete
 * @return bool True if the task was successfully deleted, false otherwise
 */
bool TaskController::deleteTask(const QString& id)
{
    // Remove from model
    if (!m_taskModel->removeTask(id)) {
        return false;
    }

    // Delete from database
    if (!DatabaseManager::instance().deleteTask(id)) {
        return false;
    }

    // Notify listeners about the change
    emit tasksChanged();
    return true;
}

/**
 * @brief Delete all completed tasks
 * 
 * Removes all tasks that have been marked as completed from
 * both the model and the database.
 * 
 * @return bool True if all completed tasks were successfully deleted, false otherwise
 */
bool TaskController::deleteCompletedTasks()
{
    QList<Task> tasks = m_taskModel->getTasks();
    QList<QString> completedTaskIds;

    // Collect IDs of completed tasks
    for (const Task& task : tasks) {
        if (task.isCompleted()) {
            completedTaskIds.append(task.id());
        }
    }

    // Delete each completed task
    for (const QString& id : completedTaskIds) {
        if (!deleteTask(id)) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Move a task to a new position
 * 
 * Reorders tasks by moving a task from one position to another.
 * The change is applied to both the model and the database.
 * 
 * @param fromRow The current row index of the task
 * @param toRow The target row index for the task
 * @return bool True if the task was successfully moved, false otherwise
 */
bool TaskController::moveTask(int fromRow, int toRow)
{
    // Validate row indices
    if (fromRow < 0 || fromRow >= m_taskModel->rowCount() ||
        toRow < 0 || toRow > m_taskModel->rowCount()) {
        qDebug() << "Invalid move operation: from=" << fromRow << ", to=" << toRow;
        return false;
    }

    // If source and destination are the same, no move needed
    if (fromRow == toRow) {
        return true;
    }

    qDebug() << "Moving task from row" << fromRow << "to row" << toRow;

    // Move the task in the model
    if (!m_taskModel->moveTask(fromRow, toRow)) {
        return false;
    }

    // Save all tasks to update their order in the database
    if (!saveTasks()) {
        return false;
    }

    // Notify listeners about the change
    emit tasksChanged();
    return true;
}

/**
 * @brief Filter tasks by category
 * 
 * Shows only tasks that belong to the specified category.
 * 
 * @param categoryId The ID of the category to filter by
 */
void TaskController::filterByCategory(const QString& categoryId)
{
    m_taskModel->filterByCategory(categoryId);
}

/**
 * @brief Clear any active filters
 * 
 * Resets all filters to show all tasks.
 */
void TaskController::clearFilter()
{
    m_taskModel->clearFilter();
}

/**
 * @brief Sort tasks by due date
 * 
 * @param ascending If true, sorts from earliest to latest; if false, from latest to earliest
 */
void TaskController::sortByDueDate(bool ascending)
{
    m_taskModel->sortByDueDate(ascending);
}

/**
 * @brief Sort tasks by priority
 * 
 * @param ascending If true, sorts from lowest to highest; if false, from highest to lowest
 */
void TaskController::sortByPriority(bool ascending)
{
    m_taskModel->sortByPriority(ascending);
}

/**
 * @brief Load tasks from the database
 * 
 * Retrieves all tasks from the database and populates the model.
 * 
 * @return bool True if tasks were successfully loaded, false otherwise
 */
bool TaskController::loadTasks()
{
    QList<Task> tasks = DatabaseManager::instance().loadTasks();
    m_taskModel->setTasks(tasks);
    emit tasksChanged();
    return true;
}

/**
 * @brief Save tasks to the database
 * 
 * Persists all tasks in the model to the database.
 * 
 * @return bool True if tasks were successfully saved, false otherwise
 */
bool TaskController::saveTasks()
{
    return DatabaseManager::instance().saveTasks(m_taskModel->getTasks());
}
