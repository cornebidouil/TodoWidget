/**
 * @file category.h
 * @brief Definition of the Category model class
 * 
 * This file defines the Category class which represents a task category or group
 * in the application. Categories allow users to organize their tasks and apply
 * different visual styles based on category.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QString>
#include <QColor>
#include <QJsonObject>

/**
 * @class Category
 * @brief Represents a task category
 * 
 * The Category class encapsulates all data related to a task category.
 * Categories allow tasks to be grouped and visually distinguished.
 * It provides methods for accessing and modifying category properties,
 * as well as serialization to/from JSON for storage and interchange.
 */
class Category {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty category with default values.
     */
    Category();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a new category with the specified name and color.
     * 
     * @param name The name of the category
     * @param color The color associated with the category
     * @param isDefault Whether this category is a default category
     */
    Category(const QString& name, const QColor& color, bool isDefault = false);

    /**
     * @brief Get the category's unique identifier
     * @return QString The category ID
     */
    QString id() const { return m_id; }
    
    /**
     * @brief Get the category's name
     * @return QString The category name
     */
    QString name() const { return m_name; }
    
    /**
     * @brief Get the category's color
     * @return QColor The color associated with the category
     */
    QColor color() const { return m_color; }
    
    /**
     * @brief Check if this is a default category
     * @return bool True if this is a default category, false otherwise
     */
    bool isDefault() const { return m_isDefault; }

    /**
     * @brief Set the category's unique identifier
     * @param id The unique identifier to set
     */
    void setId(const QString& id) { m_id = id; }
    
    /**
     * @brief Set the category's name
     * @param name The name to set
     */
    void setName(const QString& name) { m_name = name; }
    
    /**
     * @brief Set the category's color
     * @param color The color to set
     */
    void setColor(const QColor& color) { m_color = color; }
    
    /**
     * @brief Set whether this is a default category
     * @param isDefault True to mark as default, false otherwise
     */
    void setDefault(bool isDefault) { m_isDefault = isDefault; }

    /**
     * @brief Convert the category to a JSON object
     * 
     * Serializes all category properties to a JSON object for storage or transmission.
     * 
     * @return QJsonObject The JSON representation of the category
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Create a category from a JSON object
     * 
     * Static factory method to deserialize a category from its JSON representation.
     * 
     * @param json The JSON object containing category data
     * @return Category A new Category instance with properties from the JSON object
     */
    static Category fromJson(const QJsonObject& json);

private:
    QString m_id;       ///< Unique identifier
    QString m_name;     ///< Category name
    QColor m_color;     ///< Color for visual identification
    bool m_isDefault;   ///< Whether this is a default category
};
