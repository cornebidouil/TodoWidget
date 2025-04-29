/**
 * @file categorycontroller.cpp
 * @brief Implementation of the CategoryController class
 * 
 * This file implements the CategoryController class which manages operations on Category objects
 * and coordinates between the CategoryModel and database. It handles CRUD operations
 * and persistence of categories.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "categorycontroller.h"
#include <QDebug>

/**
 * @brief Constructor
 * 
 * Creates a new CategoryController connected to the specified CategoryModel
 * and sets up signal connections to ensure changes in the model trigger updates.
 * 
 * @param model Pointer to the CategoryModel to be managed
 * @param parent Optional parent QObject
 */
CategoryController::CategoryController(CategoryModel* model, QObject* parent)
    : QObject(parent), m_categoryModel(model)
{
    // Connect to model signals to ensure we catch all category changes
    connect(m_categoryModel, &QAbstractItemModel::dataChanged, 
            this, &CategoryController::onModelChanged);
    connect(m_categoryModel, &QAbstractItemModel::rowsInserted, 
            this, &CategoryController::onModelChanged);
    connect(m_categoryModel, &QAbstractItemModel::rowsRemoved, 
            this, &CategoryController::onModelChanged);
    connect(m_categoryModel, &QAbstractItemModel::modelReset, 
            this, &CategoryController::onModelChanged);
}

/**
 * @brief Add a new category
 * 
 * Creates and adds a new category with the specified name and color.
 * The category is added to both the model and the database.
 * 
 * @param name The name of the category
 * @param color The color for the category
 * @return bool True if the category was successfully added, false otherwise
 */
bool CategoryController::addCategory(const QString& name, const QColor& color)
{
    // Validate required fields
    if (name.isEmpty()) {
        return false;
    }

    // Create the category with the specified properties
    Category category(name, color);
    m_categoryModel->addCategory(category);

    qDebug() << "Added category:" << name;

    // Save to database
    if (!DatabaseManager::instance().saveCategory(category)) {
        return false;
    }

    // Notify listeners about the change
    emit categoriesChanged();
    return true;
}

/**
 * @brief Update an existing category
 * 
 * Updates the properties of an existing category identified by its ID.
 * The changes are applied to both the model and the database.
 * 
 * @param id The ID of the category to update
 * @param name The new name
 * @param color The new color
 * @return bool True if the category was successfully updated, false otherwise
 */
bool CategoryController::updateCategory(const QString& id, const QString& name, const QColor& color)
{
    // Validate required fields
    if (name.isEmpty()) {
        return false;
    }

    // Get the existing category
    Category category = m_categoryModel->getCategory(id);
    if (category.id().isEmpty()) {
        return false;  // Category not found
    }

    // Update category properties
    category.setName(name);
    category.setColor(color);

    // Find and update the category in the model
    for (int i = 0; i < m_categoryModel->rowCount(); ++i) {
        QModelIndex index = m_categoryModel->index(i, 0);
        if (m_categoryModel->data(index, CategoryModel::IdRole).toString() == id) {
            m_categoryModel->setData(index, name, CategoryModel::NameRole);
            m_categoryModel->setData(index, color, CategoryModel::ColorRole);
            break;
        }
    }

    qDebug() << "Updated category:" << category.name() << category.color();

    // Save to database
    if (!DatabaseManager::instance().saveCategory(category)) {
        return false;
    }

    // Notify listeners about the change
    emit categoriesChanged();
    return true;
}

/**
 * @brief Delete a category
 * 
 * Removes a category identified by its ID from the model and database.
 * Default categories cannot be deleted.
 * 
 * @param id The ID of the category to delete
 * @return bool True if the category was successfully deleted, false otherwise
 */
bool CategoryController::deleteCategory(const QString& id)
{
    // Get the category and check if it's a default category
    Category category = m_categoryModel->getCategory(id);
    if (category.id().isEmpty() || category.isDefault()) {
        return false; // Can't delete default categories
    }

    // Remove from model
    if (!m_categoryModel->removeCategory(id)) {
        return false;
    }

    // Delete from database
    if (!DatabaseManager::instance().deleteCategory(id)) {
        return false;
    }

    // Notify listeners about the change
    emit categoriesChanged();
    return true;
}

/**
 * @brief Get a category by ID
 * 
 * Retrieves a category by its unique identifier.
 * 
 * @param id The ID of the category to retrieve
 * @return Category The requested category
 */
Category CategoryController::getCategory(const QString& id) const
{
    return m_categoryModel->getCategory(id);
}

/**
 * @brief Load categories from the database
 * 
 * Retrieves all categories from the database and populates the model.
 * If no categories are found, creates default categories.
 * 
 * @return bool True if categories were successfully loaded, false otherwise
 */
bool CategoryController::loadCategories()
{
    QList<Category> categories = DatabaseManager::instance().loadCategories();
    m_categoryModel->setCategories(categories);

    // If no categories were loaded, create defaults
    if (categories.isEmpty()) {
        ensureDefaultCategories();
    }

    // Notify listeners about the change
    emit categoriesChanged();
    return true;
}

/**
 * @brief Save categories to the database
 * 
 * Persists all categories in the model to the database.
 * 
 * @return bool True if categories were successfully saved, false otherwise
 */
bool CategoryController::saveCategories()
{
    return DatabaseManager::instance().saveCategories(m_categoryModel->getCategories());
}

/**
 * @brief Ensure default categories exist
 * 
 * Checks if default categories exist and creates them if they don't.
 * This ensures the application always has at least a basic set of
 * categories available.
 */
void CategoryController::ensureDefaultCategories()
{
    m_categoryModel->ensureDefaultCategories();
    saveCategories();
    emit categoriesChanged();
}

/**
 * @brief Handle model changes
 * 
 * This slot is called when the model data changes to ensure
 * the controller is aware of model updates and can notify
 * listeners about changes.
 */
void CategoryController::onModelChanged()
{
    // This method will be called whenever the model changes
    // We emit categoriesChanged to ensure the UI is updated
    emit categoriesChanged();
}
