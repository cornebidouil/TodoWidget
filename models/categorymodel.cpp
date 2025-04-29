/**
 * @file categorymodel.cpp
 * @brief Implementation of the CategoryModel class
 * 
 * This file implements the CategoryModel class which manages a collection of Category objects
 * and implements the Qt Model/View architecture. It provides functionality for
 * category management and ensures default categories are available when needed.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "categorymodel.h"

/**
 * @brief Constructor
 * 
 * Creates a new empty CategoryModel.
 * 
 * @param parent Optional parent QObject
 */
CategoryModel::CategoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

/**
 * @brief Get the number of rows (categories) in the model
 * 
 * Returns the number of categories currently in the model.
 * 
 * @param parent Parent model index (unused in list models)
 * @return int Number of categories in the model
 */
int CategoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_categories.size();
}

/**
 * @brief Get data for a specific category and role
 * 
 * Returns the requested data from the category at the specified index.
 * The data returned depends on the role requested.
 * 
 * @param index Model index identifying the category
 * @param role Data role to retrieve
 * @return QVariant The requested data
 */
QVariant CategoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Category &category = m_categories[index.row()];

    // Return the requested data based on the role
    switch (role) {
        case Qt::DisplayRole:
        case NameRole:
            return category.name();
        case ColorRole:
            return category.color();
        case IsDefaultRole:
            return category.isDefault();
        case IdRole:
            return category.id();
        default:
            return QVariant();
    }
}

/**
 * @brief Set data for a specific category and role
 * 
 * Updates the specified data in the category at the given index.
 * The data updated depends on the role specified.
 * 
 * @param index Model index identifying the category
 * @param value New value to set
 * @param role Data role to modify
 * @return bool True if data was successfully set, false otherwise
 */
bool CategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    Category &category = m_categories[index.row()];

    // Update the requested data based on the role
    switch (role) {
        case NameRole:
            category.setName(value.toString());
            break;
        case ColorRole:
            category.setColor(value.value<QColor>());
            break;
        case IsDefaultRole:
            category.setDefault(value.toBool());
            break;
        default:
            return false;
    }

    // Notify views that the data has changed
    emit dataChanged(index, index, {role});
    return true;
}

/**
 * @brief Get item flags for a specific index
 * 
 * Returns the flags for the category at the specified index.
 * All categories are enabled, selectable, and editable.
 * 
 * @param index Model index to get flags for
 * @return Qt::ItemFlags Flags for the specified category
 */
Qt::ItemFlags CategoryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

/**
 * @brief Get role names used for QML integration
 * 
 * Returns a mapping from role values to role names for use in QML.
 * 
 * @return QHash<int, QByteArray> Mapping from role values to role names
 */
QHash<int, QByteArray> CategoryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[ColorRole] = "color";
    roles[IsDefaultRole] = "isDefault";
    return roles;
}

/**
 * @brief Add a category to the model
 * 
 * Adds a new category to the model.
 * 
 * @param category The category to add
 */
void CategoryModel::addCategory(const Category &category)
{
    beginInsertRows(QModelIndex(), m_categories.size(), m_categories.size());
    m_categories.append(category);
    endInsertRows();
}

/**
 * @brief Remove a category from the model
 * 
 * Removes the category with the specified ID from the model.
 * Default categories cannot be removed.
 * 
 * @param id ID of the category to remove
 * @return bool True if the category was found and removed, false otherwise
 */
bool CategoryModel::removeCategory(const QString &id)
{
    for (int i = 0; i < m_categories.size(); ++i) {
        if (m_categories[i].id() == id) {
            // Don't allow removing default categories
            if (m_categories[i].isDefault()) {
                return false;
            }

            beginRemoveRows(QModelIndex(), i, i);
            m_categories.removeAt(i);
            endRemoveRows();
            return true;
        }
    }
    return false;
}

/**
 * @brief Get a category by ID
 * 
 * Retrieves the category with the specified ID.
 * 
 * @param id ID of the category to retrieve
 * @return Category The requested category, or an empty category if not found
 */
Category CategoryModel::getCategory(const QString &id) const
{
    for (const Category &category : m_categories) {
        if (category.id() == id) {
            return category;
        }
    }
    return Category(); // Return empty category if not found
}

/**
 * @brief Get all categories in the model
 * 
 * Returns the complete list of categories.
 * 
 * @return QList<Category> List of all categories
 */
QList<Category> CategoryModel::getCategories() const
{
    return m_categories;
}

/**
 * @brief Replace all categories in the model
 * 
 * Replaces the current category list with a new one.
 * 
 * @param categories New list of categories
 */
void CategoryModel::setCategories(const QList<Category> &categories)
{
    beginResetModel();
    m_categories = categories;
    endResetModel();
}

/**
 * @brief Create default categories if none exist
 * 
 * If the category list is empty, creates a set of default categories
 * with predefined names and colors.
 */
void CategoryModel::ensureDefaultCategories()
{
    if (m_categories.isEmpty()) {
        // Create default categories with different colors
        addCategory(Category("Work", QColor("#4285F4"), true));       // Blue
        addCategory(Category("Personal", QColor("#EA4335"), true));   // Red
        addCategory(Category("Shopping", QColor("#FBBC05"), true));   // Yellow
        addCategory(Category("Health", QColor("#34A853"), true));     // Green
        addCategory(Category("Finance", QColor("#8E44AD"), true));    // Purple
    }
}
