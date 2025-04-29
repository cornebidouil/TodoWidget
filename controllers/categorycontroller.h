/**
 * @file categorycontroller.h
 * @brief Definition of the CategoryController class
 * 
 * This file defines the CategoryController class which manages operations on Category objects
 * and coordinates between the CategoryModel and database. It handles CRUD operations
 * and persistence of categories.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QObject>
#include "../models/category.h"
#include "../models/categorymodel.h"
#include "../services/databasemanager.h"

/**
 * @class CategoryController
 * @brief Controller for category-related operations
 * 
 * The CategoryController class implements the business logic for managing categories.
 * It acts as an intermediary between the view and the model/database,
 * handling operations such as adding, updating, and deleting categories,
 * as well as persistence operations.
 */
class CategoryController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new CategoryController connected to the specified CategoryModel.
     * 
     * @param model Pointer to the CategoryModel to be managed
     * @param parent Optional parent QObject
     */
    explicit CategoryController(CategoryModel* model, QObject* parent = nullptr);

    /**
     * @brief Add a new category
     * 
     * Creates and adds a new category with the specified name and color.
     * 
     * @param name The name of the category
     * @param color The color for the category
     * @return bool True if the category was successfully added, false otherwise
     */
    bool addCategory(const QString& name, const QColor& color);
    
    /**
     * @brief Update an existing category
     * 
     * Updates the properties of an existing category identified by its ID.
     * 
     * @param id The ID of the category to update
     * @param name The new name
     * @param color The new color
     * @return bool True if the category was successfully updated, false otherwise
     */
    bool updateCategory(const QString& id, const QString& name, const QColor& color);
    
    /**
     * @brief Delete a category
     * 
     * Removes a category identified by its ID from the model and database.
     * 
     * @param id The ID of the category to delete
     * @return bool True if the category was successfully deleted, false otherwise
     */
    bool deleteCategory(const QString& id);

    /**
     * @brief Get a category by ID
     * 
     * Retrieves a category by its unique identifier.
     * 
     * @param id The ID of the category to retrieve
     * @return Category The requested category
     */
    Category getCategory(const QString& id) const;

    /**
     * @brief Load categories from the database
     * 
     * Retrieves all categories from the database and populates the model.
     * 
     * @return bool True if categories were successfully loaded, false otherwise
     */
    bool loadCategories();
    
    /**
     * @brief Save categories to the database
     * 
     * Persists all categories in the model to the database.
     * 
     * @return bool True if categories were successfully saved, false otherwise
     */
    bool saveCategories();

    /**
     * @brief Ensure default categories exist
     * 
     * Checks if default categories exist and creates them if they don't.
     * This ensures the application always has at least a basic set of
     * categories available.
     */
    void ensureDefaultCategories();

signals:
    /**
     * @brief Signal emitted when categories have been changed
     * 
     * This signal is emitted whenever categories are added, updated, or deleted,
     * allowing views to update accordingly.
     */
    void categoriesChanged();
    
private slots:
    /**
     * @brief Handle model changes
     * 
     * This slot is called when the model data changes to ensure
     * the controller is aware of model updates.
     */
    void onModelChanged();

private:
    CategoryModel* m_categoryModel;  ///< Pointer to the category model being managed
};
