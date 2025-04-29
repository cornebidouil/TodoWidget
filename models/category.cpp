/**
 * @file category.cpp
 * @brief Implementation of the Category class
 * 
 * This file implements the Category class which represents a task category.
 * It includes constructors, serialization and deserialization methods.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "category.h"
#include <QUuid>

/**
 * @brief Default constructor
 * 
 * Creates a new category with default values:
 * - Generates a unique ID using QUuid
 * - Sets color to blue
 * - Sets isDefault to false
 */
Category::Category() :
    m_id(QUuid::createUuid().toString().remove('{').remove('}')),
    m_color(Qt::blue),
    m_isDefault(false)
{
}

/**
 * @brief Parameterized constructor
 * 
 * Creates a new category with the specified name, color, and default status:
 * - Generates a unique ID using QUuid
 * - Sets the specified name, color, and default status
 * 
 * @param name The name of the category
 * @param color The color associated with the category
 * @param isDefault Whether this is a default category
 */
Category::Category(const QString& name, const QColor& color, bool isDefault) :
    m_id(QUuid::createUuid().toString().remove('{').remove('}')),
    m_name(name),
    m_color(color),
    m_isDefault(isDefault)
{
}

/**
 * @brief Convert category to JSON object
 * 
 * Serializes all category properties to a JSON object for storage or transmission.
 * Color is stored as a string using QColor's name() method.
 * 
 * @return QJsonObject containing all category properties
 */
QJsonObject Category::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["color"] = m_color.name();  // Convert QColor to string using the color name
    json["isDefault"] = m_isDefault;
    return json;
}

/**
 * @brief Create category from JSON object
 * 
 * Static factory method to deserialize a category from its JSON representation.
 * Converts the color string back to a QColor object.
 * 
 * @param json The JSON object containing category data
 * @return Category A new Category instance with properties from the JSON object
 */
Category Category::fromJson(const QJsonObject& json) {
    Category category;

    category.setId(json["id"].toString());
    category.setName(json["name"].toString());
    category.setColor(QColor(json["color"].toString()));  // Convert string back to QColor
    category.setDefault(json["isDefault"].toBool());

    return category;
}
