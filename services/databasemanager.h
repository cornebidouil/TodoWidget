/**
 * @file databasemanager.h
 * @brief Definition of the DatabaseManager class
 * 
 * This file defines the DatabaseManager singleton class which handles all database
 * operations for the TODO Widget application. It provides methods for initializing
 * the database connection and performing CRUD operations on tasks and categories.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "../models/task.h"
#include "../models/category.h"

/**
 * @class DatabaseManager
 * @brief Singleton manager for database operations
 * 
 * The DatabaseManager class provides a centralized interface for all database
 * operations in the application. It is implemented as a singleton to ensure
 * that there is only one database connection throughout the application.
 * 
 * It handles:
 * - Establishing and initializing the database connection
 * - Creating the necessary tables if they don't exist
 * - CRUD operations for tasks and categories
 * - Batch operations for saving multiple items at once
 */
class DatabaseManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     * 
     * Returns the single instance of the DatabaseManager class,
     * creating it if it doesn't already exist.
     * 
     * @return DatabaseManager& Reference to the singleton instance
     */
    static DatabaseManager& instance();

    /**
     * @brief Initialize the database connection
     * 
     * Establishes a connection to the SQLite database and creates
     * the necessary tables if they don't exist.
     * 
     * @return bool True if initialization was successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Check if the database is initialized
     * 
     * @return bool True if the database is initialized and ready for use, false otherwise
     */
    bool isInitialized() const { return m_initialized; }

    /**
     * @brief Save multiple tasks
     * 
     * Saves a list of tasks to the database, performing either
     * updates or inserts as needed.
     * 
     * @param tasks List of Task objects to save
     * @return bool True if all tasks were saved successfully, false otherwise
     */
    bool saveTasks(const QList<Task>& tasks);
    
    /**
     * @brief Load all tasks
     * 
     * Retrieves all tasks from the database.
     * 
     * @return QList<Task> List of all tasks in the database
     */
    QList<Task> loadTasks();
    
    /**
     * @brief Save a single task
     * 
     * Saves a task to the database, performing either an update
     * or an insert as needed.
     * 
     * @param task The Task object to save
     * @return bool True if the task was saved successfully, false otherwise
     */
    bool saveTask(const Task& task);
    
    /**
     * @brief Delete a task
     * 
     * Removes a task from the database by its ID.
     * 
     * @param id The ID of the task to delete
     * @return bool True if the task was deleted successfully, false otherwise
     */
    bool deleteTask(const QString& id);

    /**
     * @brief Save multiple categories
     * 
     * Saves a list of categories to the database, performing either
     * updates or inserts as needed.
     * 
     * @param categories List of Category objects to save
     * @return bool True if all categories were saved successfully, false otherwise
     */
    bool saveCategories(const QList<Category>& categories);
    
    /**
     * @brief Load all categories
     * 
     * Retrieves all categories from the database.
     * 
     * @return QList<Category> List of all categories in the database
     */
    QList<Category> loadCategories();
    
    /**
     * @brief Save a single category
     * 
     * Saves a category to the database, performing either an update
     * or an insert as needed.
     * 
     * @param category The Category object to save
     * @return bool True if the category was saved successfully, false otherwise
     */
    bool saveCategory(const Category& category);
    
    /**
     * @brief Delete a category
     * 
     * Removes a category from the database by its ID.
     * 
     * @param id The ID of the category to delete
     * @return bool True if the category was deleted successfully, false otherwise
     */
    bool deleteCategory(const QString& id);

private:
    /**
     * @brief Private constructor
     * 
     * Prevents direct instantiation to ensure singleton pattern.
     * 
     * @param parent Optional parent QObject
     */
    DatabaseManager(QObject* parent = nullptr);
    
    /**
     * @brief Private destructor
     * 
     * Closes the database connection when the manager is destroyed.
     */
    ~DatabaseManager();

    /**
     * @brief Deleted copy constructor
     * 
     * Prevents copying to ensure singleton pattern.
     */
    DatabaseManager(const DatabaseManager&) = delete;
    
    /**
     * @brief Deleted assignment operator
     * 
     * Prevents assignment to ensure singleton pattern.
     */
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    /**
     * @brief Create database tables
     * 
     * Creates the necessary tables in the database if they don't exist.
     * 
     * @return bool True if tables were created successfully, false otherwise
     */
    bool createTables();

    QSqlDatabase m_database;  ///< The SQLite database connection
    bool m_initialized;       ///< Flag indicating if the database is initialized
};
