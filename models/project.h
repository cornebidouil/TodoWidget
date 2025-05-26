/**
 * @file project.h
 * @brief Definition of the Project model class
 * 
 * This file defines the Project class which represents a project for time tracking.
 * Projects are independent entities from tasks and are used to categorize time entries.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QString>
#include <QColor>
#include <QJsonObject>

/**
 * @class Project
 * @brief Represents a project for time tracking
 * 
 * The Project class encapsulates all data related to a project.
 * Projects are used to organize time entries and can have properties
 * like name, color, and description.
 */
class Project {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty project with default values.
     */
    Project();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a new project with the specified name and color.
     * 
     * @param name The name of the project
     * @param color The color associated with the project
     */
    Project(const QString& name, const QColor& color);
    
    /**
     * @brief Full constructor
     * 
     * Creates a new project with all properties specified.
     * 
     * @param name The name of the project
     * @param color The color associated with the project
     * @param description The description of the project
     */
    Project(const QString& name, const QColor& color, const QString& description);

    /**
     * @brief Get the project's unique identifier
     * @return QString The project ID
     */
    QString id() const { return m_id; }
    
    /**
     * @brief Get the project's name
     * @return QString The project name
     */
    QString name() const { return m_name; }
    
    /**
     * @brief Get the project's color
     * @return QColor The color associated with the project
     */
    QColor color() const { return m_color; }
    
    /**
     * @brief Get the project's description
     * @return QString The project description
     */
    QString description() const { return m_description; }
    
    /**
     * @brief Get whether the project is active
     * @return bool True if the project is active, false otherwise
     */
    bool isActive() const { return m_isActive; }

    /**
     * @brief Set the project's unique identifier
     * @param id The unique identifier to set
     */
    void setId(const QString& id) { m_id = id; }
    
    /**
     * @brief Set the project's name
     * @param name The name to set
     */
    void setName(const QString& name) { m_name = name; }
    
    /**
     * @brief Set the project's color
     * @param color The color to set
     */
    void setColor(const QColor& color) { m_color = color; }
    
    /**
     * @brief Set the project's description
     * @param description The description to set
     */
    void setDescription(const QString& description) { m_description = description; }
    
    /**
     * @brief Set whether the project is active
     * @param active True to mark as active, false for inactive
     */
    void setActive(bool active) { m_isActive = active; }

    /**
     * @brief Convert the project to a JSON object
     * 
     * Serializes all project properties to a JSON object for storage or transmission.
     * 
     * @return QJsonObject The JSON representation of the project
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Create a project from a JSON object
     * 
     * Static factory method to deserialize a project from its JSON representation.
     * 
     * @param json The JSON object containing project data
     * @return Project A new Project instance with properties from the JSON object
     */
    static Project fromJson(const QJsonObject& json);

private:
    QString m_id;            ///< Unique identifier
    QString m_name;          ///< Project name
    QColor m_color;          ///< Color for visual identification
    QString m_description;   ///< Optional description
    bool m_isActive;         ///< Whether the project is active
};
