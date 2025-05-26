/**
 * @file databasemanager.h
 * @brief Definition of the DatabaseManager class
 * 
 * This file defines the DatabaseManager singleton class which handles all database
 * operations for the TODO Widget application. It provides methods for initializing
 * the database connection and performing CRUD operations on tasks, categories, and time entries.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "../models/task.h"
#include "../models/category.h"
#include "../models/timeentry.h"
#include "../models/project.h"

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

    /**
     * @brief Save multiple time entries
     * 
     * Saves a list of time entries to the database, performing either
     * updates or inserts as needed.
     * 
     * @param timeEntries List of TimeEntry objects to save
     * @return bool True if all time entries were saved successfully, false otherwise
     */
    bool saveTimeEntries(const QList<TimeEntry>& timeEntries);
    
    /**
     * @brief Load all time entries
     * 
     * Retrieves all time entries from the database.
     * 
     * @return QList<TimeEntry> List of all time entries in the database
     */
    QList<TimeEntry> loadTimeEntries();
    
    /**
     * @brief Save a single time entry
     * 
     * Saves a time entry to the database, performing either an update
     * or an insert as needed.
     * 
     * @param timeEntry The TimeEntry object to save
     * @return bool True if the time entry was saved successfully, false otherwise
     */
    bool saveTimeEntry(const TimeEntry& timeEntry);
    
    /**
     * @brief Delete a time entry
     * 
     * Removes a time entry from the database by its ID.
     * 
     * @param id The ID of the time entry to delete
     * @return bool True if the time entry was deleted successfully, false otherwise
     */
    bool deleteTimeEntry(const QString& id);

    /**
     * @brief Save multiple projects
     * 
     * Saves a list of projects to the database, performing either
     * updates or inserts as needed.
     * 
     * @param projects List of Project objects to save
     * @return bool True if all projects were saved successfully, false otherwise
     */
    bool saveProjects(const QList<Project>& projects);
    
    /**
     * @brief Load all projects
     * 
     * Retrieves all projects from the database.
     * 
     * @return QList<Project> List of all projects in the database
     */
    QList<Project> loadProjects();
    
    /**
     * @brief Save a single project
     * 
     * Saves a project to the database, performing either an update
     * or an insert as needed.
     * 
     * @param project The Project object to save
     * @return bool True if the project was saved successfully, false otherwise
     */
    bool saveProject(const Project& project);
    
    /**
     * @brief Delete a project
     * 
     * Removes a project from the database by its ID.
     * 
     * @param id The ID of the project to delete
     * @return bool True if the project was deleted successfully, false otherwise
     */
    bool deleteProject(const QString& id);

    /**
     * @brief Get time entries for a project
     * 
     * Retrieves all time entries associated with a specific project.
     * 
     * @param projectId The ID of the project to get time entries for
     * @return QList<TimeEntry> List of time entries for the project
     */
    QList<TimeEntry> getTimeEntriesForProject(const QString& projectId) const;

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

    /**
     * @brief Create the tasks table
     * 
     * Creates the tasks table if it doesn't exist.
     * 
     * @return bool True if the table was created successfully, false otherwise
     */
    bool createTasksTable();

    /**
     * @brief Create the categories table
     * 
     * Creates the categories table if it doesn't exist.
     * 
     * @return bool True if the table was created successfully, false otherwise
     */
    bool createCategoriesTable();

    /**
     * @brief Create the projects table
     * 
     * Creates the projects table if it doesn't exist.
     * 
     * @return bool True if the table was created successfully, false otherwise
     */
    bool createProjectsTable();

    /**
     * @brief Create the time_entries table
     * 
     * Creates the time_entries table if it doesn't exist.
     * 
     * @return bool True if the table was created successfully, false otherwise
     */
    bool createTimeEntriesTable();

    QSqlDatabase m_database;  ///< The SQLite database connection
    bool m_initialized;       ///< Flag indicating if the database is initialized
};
