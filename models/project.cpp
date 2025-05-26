/**
 * @file project.cpp
 * @brief Implementation of the Project model class
 * 
 * The Project class represents a project entity in the application.
 * Projects are used to categorize and organize time entries, allowing
 * users to track time spent on different activities.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "project.h"
#include <QUuid>

/**
 * @brief Default constructor for Project
 * 
 * Creates a new Project with a generated UUID, default blue color,
 * and active status. All other properties are empty.
 */
Project::Project()
    : m_id(QUuid::createUuid().toString().remove('{').remove('}'))
    , m_name("")
    , m_color(Qt::blue)
    , m_description("")
    , m_isActive(true)
{
}

/**
 * @brief Constructor with name and color
 * 
 * Creates a new Project with the specified name and color.
 * A UUID is generated, the description is empty, and the project is active.
 * 
 * @param name The name of the project
 * @param color The color associated with the project
 */
Project::Project(const QString& name, const QColor& color)
    : m_id(QUuid::createUuid().toString().remove('{').remove('}'))
    , m_name(name)
    , m_color(color)
    , m_description("")
    , m_isActive(true)
{
}

/**
 * @brief Constructor with name, color, and description
 * 
 * Creates a new Project with the specified name, color, and description.
 * A UUID is generated and the project is active.
 * 
 * @param name The name of the project
 * @param color The color associated with the project
 * @param description The project description
 */
Project::Project(const QString& name, const QColor& color, const QString& description)
    : m_id(QUuid::createUuid().toString().remove('{').remove('}'))
    , m_name(name)
    , m_color(color)
    , m_description(description)
    , m_isActive(true)
{
}

/**
 * @brief Convert the Project to a JSON object
 * 
 * Serializes all project properties to a JSON object for storage.
 * 
 * @return QJsonObject containing the project data
 */
QJsonObject Project::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["color"] = m_color.name(QColor::HexArgb);
    json["description"] = m_description;
    json["isActive"] = m_isActive;
    return json;
}

/**
 * @brief Create a Project from a JSON object
 * 
 * Static factory method that deserializes a Project from a JSON object.
 * 
 * @param json The JSON object containing project data
 * @return A new Project instance with properties from the JSON
 */
Project Project::fromJson(const QJsonObject& json)
{
    Project project;
    
    if (json.contains("id")) {
        project.setId(json["id"].toString());
    }
    
    if (json.contains("name")) {
        project.setName(json["name"].toString());
    }
    
    if (json.contains("color")) {
        project.setColor(QColor(json["color"].toString()));
    }
    
    if (json.contains("description")) {
        project.setDescription(json["description"].toString());
    }
    
    if (json.contains("isActive")) {
        project.setActive(json["isActive"].toBool());
    }
    
    return project;
}
