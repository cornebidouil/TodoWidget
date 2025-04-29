/**
 * @file categorymodel.h
 * @brief Definition of the CategoryModel class
 * 
 * This file defines the CategoryModel class which manages a collection of Category objects
 * and implements the Qt Model/View architecture for displaying categories in views.
 * It provides methods for category management and ensures default categories are available.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QAbstractListModel>
#include "category.h"

/**
 * @class CategoryModel
 * @brief Model for managing a collection of categories
 * 
 * The CategoryModel class implements QAbstractListModel to provide a model
 * for category data that can be used with Qt's Model/View architecture.
 * It manages a collection of Category objects and provides methods for
 * adding, removing, and modifying categories. It also ensures that
 * default categories are available when needed.
 */
class CategoryModel : public QAbstractListModel {
    Q_OBJECT

public:
    /**
     * @brief Custom roles for accessing category data
     * 
     * These roles extend Qt's standard model roles to provide
     * access to specific category properties in views and delegates.
     */
    enum CategoryRoles {
        IdRole = Qt::UserRole + 1,  ///< Role for accessing the category ID
        NameRole,                   ///< Role for accessing the category name
        ColorRole,                  ///< Role for accessing the category color
        IsDefaultRole               ///< Role for accessing the category default status
    };

    /**
     * @brief Constructor
     * 
     * Creates a new empty CategoryModel.
     * 
     * @param parent Optional parent QObject
     */
    explicit CategoryModel(QObject *parent = nullptr);

    // QAbstractItemModel implementation
    /**
     * @brief Get the number of rows (categories) in the model
     * 
     * @param parent Parent model index (unused in list models)
     * @return int Number of categories in the model
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

    // Category management methods
    /**
     * @brief Add a category to the model
     * 
     * @param category The category to add
     */
    void addCategory(const Category &category);
    
    /**
     * @brief Remove a category from the model
     * 
     * @param id ID of the category to remove
     * @return bool True if the category was found and removed
     */
    bool removeCategory(const QString &id);
    
    /**
     * @brief Get a category by ID
     * 
     * @param id ID of the category to retrieve
     * @return Category The requested category
     */
    Category getCategory(const QString &id) const;
    
    /**
     * @brief Get all categories in the model
     * 
     * @return QList<Category> List of all categories
     */
    QList<Category> getCategories() const;
    
    /**
     * @brief Replace all categories in the model
     * 
     * @param categories New list of categories
     */
    void setCategories(const QList<Category> &categories);

    /**
     * @brief Create default categories if none exist
     * 
     * Ensures that at least a set of default categories are available
     * in the model, creating them if necessary.
     */
    void ensureDefaultCategories();

private:
    QList<Category> m_categories;  ///< All categories in the model
};
