/**
 * @file task.h
 * @brief Definition of the Task model class
 * 
 * This file defines the Task class which represents a single to-do item in the application.
 * It contains all the properties of a task including its identifier, title, description,
 * completion status, dates, category, priority, and display order.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>

/**
 * @class Task
 * @brief Represents a single to-do task item
 * 
 * The Task class encapsulates all data related to a single task in the to-do list.
 * It provides methods for accessing and modifying task properties, as well as
 * serialization to/from JSON for storage and interchange.
 */
class Task {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty task with default values.
     */
    Task();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a new task with the specified title and category.
     * 
     * @param title The title of the task
     * @param categoryId The ID of the category this task belongs to
     */
    Task(const QString& title, const QString& categoryId);

    /**
     * @brief Get the task's unique identifier
     * @return QString The task ID
     */
    QString id() const { return m_id; }
    
    /**
     * @brief Get the task's title
     * @return QString The task title
     */
    QString title() const { return m_title; }
    
    /**
     * @brief Get the task's description
     * @return QString The task description
     */
    QString description() const { return m_description; }
    
    /**
     * @brief Get the task's completion status
     * @return bool True if the task is completed, false otherwise
     */
    bool isCompleted() const { return m_isCompleted; }
    
    /**
     * @brief Get the task's creation date
     * @return QDateTime The date and time when the task was created
     */
    QDateTime createdDate() const { return m_createdDate; }
    
    /**
     * @brief Get the task's due date
     * @return QDateTime The date and time when the task is due (can be null)
     */
    QDateTime dueDate() const { return m_dueDate; }
    
    /**
     * @brief Get the ID of the category this task belongs to
     * @return QString The category ID
     */
    QString categoryId() const { return m_categoryId; }
    
    /**
     * @brief Get the task's priority level
     * @return int The priority level (1-5, where higher is more important)
     */
    int priority() const { return m_priority; }
    
    /**
     * @brief Get the task's display order
     * @return int The display order for custom sorting
     */
    int displayOrder() const { return m_displayOrder; }

    /**
     * @brief Set the task's unique identifier
     * @param id The unique identifier to set
     */
    void setId(const QString& id) { m_id = id; }
    
    /**
     * @brief Set the task's title
     * @param title The title to set
     */
    void setTitle(const QString& title) { m_title = title; }
    
    /**
     * @brief Set the task's description
     * @param description The description to set
     */
    void setDescription(const QString& description) { m_description = description; }
    
    /**
     * @brief Set the task's completion status
     * @param completed True to mark as completed, false otherwise
     */
    void setCompleted(bool completed) { m_isCompleted = completed; }
    
    /**
     * @brief Set the task's creation date
     * @param date The creation date to set
     */
    void setCreatedDate(const QDateTime& date) { m_createdDate = date; }
    
    /**
     * @brief Set the task's due date
     * @param date The due date to set (can be null for no due date)
     */
    void setDueDate(const QDateTime& date) { m_dueDate = date; }
    
    /**
     * @brief Set the category this task belongs to
     * @param categoryId The ID of the category
     */
    void setCategoryId(const QString& categoryId) { m_categoryId = categoryId; }
    
    /**
     * @brief Set the task's priority level
     * @param priority The priority level (1-5, where higher is more important)
     */
    void setPriority(int priority) { m_priority = priority; }
    
    /**
     * @brief Set the task's display order
     * @param order The display order for custom sorting
     */
    void setDisplayOrder(int order) { m_displayOrder = order; }

    /**
     * @brief Convert the task to a JSON object
     * 
     * Serializes all task properties to a JSON object for storage or transmission.
     * 
     * @return QJsonObject The JSON representation of the task
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Create a task from a JSON object
     * 
     * Static factory method to deserialize a task from its JSON representation.
     * 
     * @param json The JSON object containing task data
     * @return Task A new Task instance with properties from the JSON object
     */
    static Task fromJson(const QJsonObject& json);

private:
    QString m_id;             ///< Unique identifier
    QString m_title;          ///< Task title
    QString m_description;    ///< Optional description
    bool m_isCompleted;       ///< Completion status
    QDateTime m_createdDate;  ///< Creation timestamp
    QDateTime m_dueDate;      ///< Optional due date (can be null)
    QString m_categoryId;     ///< Reference to category
    int m_priority;           ///< Task priority (1-5)
    int m_displayOrder;       ///< Display order for custom sorting
};
