/**
 * @file task.cpp
 * @brief Implementation of the Task class
 * 
 * This file implements the Task class which represents a single to-do item.
 * It includes constructors, serialization and deserialization methods.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "task.h"
#include <QUuid>
#include <QJsonObject>

/**
 * @brief Default constructor
 * 
 * Creates a new task with default values:
 * - Generates a unique ID using QUuid
 * - Sets completed status to false
 * - Sets creation date to current date/time
 * - Sets priority to medium (3)
 * - Sets display order to -1 (not ordered yet)
 */
Task::Task() :
    m_id(QUuid::createUuid().toString().remove('{').remove('}')),
    m_isCompleted(false),
    m_createdDate(QDateTime::currentDateTime()),
    m_priority(3), // Default medium priority
    m_displayOrder(-1) // Default display order -1 (not ordered yet)
{
}

/**
 * @brief Parameterized constructor
 * 
 * Creates a new task with the specified title and category, plus default values:
 * - Generates a unique ID using QUuid
 * - Sets the specified title and category ID
 * - Sets completed status to false
 * - Sets creation date to current date/time
 * - Sets priority to medium (3)
 * - Sets display order to -1 (not ordered yet)
 * 
 * @param title The title of the task
 * @param categoryId The ID of the category this task belongs to
 */
Task::Task(const QString& title, const QString& categoryId) :
    m_id(QUuid::createUuid().toString().remove('{').remove('}')),
    m_title(title),
    m_isCompleted(false),
    m_createdDate(QDateTime::currentDateTime()),
    m_categoryId(categoryId),
    m_priority(3), // Default medium priority
    m_displayOrder(-1) // Default display order -1 (not ordered yet)
{
}

/**
 * @brief Convert task to JSON object
 * 
 * Serializes all task properties to a JSON object for storage or transmission.
 * Due date is only included if it's valid (has been set).
 * 
 * @return QJsonObject containing all task properties
 */
QJsonObject Task::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["title"] = m_title;
    json["description"] = m_description;
    json["isCompleted"] = m_isCompleted;
    json["createdDate"] = m_createdDate.toString(Qt::ISODate);

    // Only include due date if it's valid (has been set)
    if (m_dueDate.isValid()) {
        json["dueDate"] = m_dueDate.toString(Qt::ISODate);
    }

    json["categoryId"] = m_categoryId;
    json["priority"] = m_priority;
    json["displayOrder"] = m_displayOrder;

    return json;
}

/**
 * @brief Create task from JSON object
 * 
 * Static factory method to deserialize a task from its JSON representation.
 * Handles backward compatibility for older data formats that might not 
 * include all current properties (e.g., displayOrder).
 * 
 * @param json The JSON object containing task data
 * @return Task A new Task instance with properties from the JSON object
 */
Task Task::fromJson(const QJsonObject& json) {
    Task task;

    task.setId(json["id"].toString());
    task.setTitle(json["title"].toString());
    task.setDescription(json["description"].toString());
    task.setCompleted(json["isCompleted"].toBool());
    task.setCreatedDate(QDateTime::fromString(json["createdDate"].toString(), Qt::ISODate));

    // Only set due date if it exists and is not empty
    if (json.contains("dueDate") && !json["dueDate"].toString().isEmpty()) {
        task.setDueDate(QDateTime::fromString(json["dueDate"].toString(), Qt::ISODate));
    }

    task.setCategoryId(json["categoryId"].toString());
    task.setPriority(json["priority"].toInt());

    // Handle display order with backward compatibility for older data
    if (json.contains("displayOrder")) {
        task.setDisplayOrder(json["displayOrder"].toInt());
    } else {
        task.setDisplayOrder(-1); // Default for older data
    }

    return task;
}
