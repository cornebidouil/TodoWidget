/**
 * @file databasemanager.cpp
 * @brief Implementation of the DatabaseManager class
 * 
 * This file implements the DatabaseManager class which provides a singleton interface
 * for database operations. It handles database initialization, connection management,
 * and CRUD operations for tasks and categories using SQLite.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "databasemanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

/**
 * @brief Get singleton instance
 * 
 * Returns a reference to the singleton DatabaseManager instance.
 * Creates the instance if it doesn't exist yet.
 * 
 * @return DatabaseManager& Reference to the singleton instance
 */
DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

/**
 * @brief Constructor
 * 
 * Creates a new DatabaseManager instance.
 * The database is not initialized in the constructor.
 * 
 * @param parent Optional parent QObject
 */
DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent), m_initialized(false)
{
}

/**
 * @brief Destructor
 * 
 * Closes the database connection if it's open.
 */
DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

/**
 * @brief Initialize the database
 * 
 * Sets up the database connection, creates the database file if it doesn't exist,
 * and creates the required tables. This method must be called before any other
 * database operations can be performed.
 * 
 * @return bool True if initialization was successful, false otherwise
 */
bool DatabaseManager::initialize()
{
    try {
        // If already initialized, skip
        if (m_initialized)
            return true;

        m_database = QSqlDatabase::addDatabase("QSQLITE");

        // Get application data path
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataPath);

        // Create directory if it doesn't exist
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                qWarning() << "Failed to create data directory:" << dataPath;
                return false;
            }
        }

        // Set database path
        QString dbPath = dir.filePath("todowidget.db");

        // Check if database file already exists
        bool dbExists = QFile::exists(dbPath);

        m_database.setDatabaseName(dbPath);

        if (!m_database.open()) {
            qWarning() << "Failed to open database:" << m_database.lastError().text();
            return false;
        }

        // Create tables if they don't exist
        if (!dbExists) {
            if (!createTables()) {
                return false;
            }
        } else {
            qDebug() << "Database already exists. Connected to existing database.";
        }

        // Create default categories if none exist
        /*QSqlQuery countQuery("SELECT COUNT(*) FROM categories");
        if (countQuery.next() && countQuery.value(0).toInt() == 0) {
            createDefaultCategories();
        }*/

        m_initialized = true;

        return true;
    } catch (const std::exception& e) {
        qWarning() << "Exception during database initialization:" << e.what();
        return false;
    } catch (...) {
        qWarning() << "Unknown exception during database initialization";
        return false;
    }
}

/**
 * @brief Create database tables
 * 
 * Creates the required tables in the database if they don't already exist.
 * This includes tables for tasks and categories.
 * 
 * @return bool True if tables were created successfully, false otherwise
 */
bool DatabaseManager::createTables()
{
    QSqlQuery query;

    // Create tasks table
    if (!query.exec("CREATE TABLE IF NOT EXISTS tasks ("
                   "id TEXT PRIMARY KEY, "
                   "title TEXT NOT NULL, "
                   "description TEXT, "
                   "completed INTEGER, "
                   "created_date TEXT, "
                   "due_date TEXT, "
                   "category_id TEXT, "
                   "priority INTEGER, "
                   "display_order INTEGER)")) {
        qWarning() << "Failed to create tasks table:" << query.lastError().text();
        return false;
    }

    // Create categories table
    if (!query.exec("CREATE TABLE IF NOT EXISTS categories ("
                   "id TEXT PRIMARY KEY, "
                   "name TEXT NOT NULL, "
                   "color TEXT, "
                   "is_default INTEGER)")) {
        qWarning() << "Failed to create categories table:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Save multiple tasks
 * 
 * Saves a list of tasks to the database in a single transaction.
 * This method deletes all existing tasks and replaces them with the new list.
 * 
 * @param tasks List of Task objects to save
 * @return bool True if all tasks were saved successfully, false otherwise
 */
bool DatabaseManager::saveTasks(const QList<Task>& tasks)
{
    if (!m_initialized) {
        return false;
    }

    // Start a transaction for better performance
    m_database.transaction();

    // Clear existing tasks
    QSqlQuery clearQuery;
    if (!clearQuery.exec("DELETE FROM tasks")) {
        m_database.rollback();
        return false;
    }

    // Insert all tasks
    QSqlQuery query;
    query.prepare("INSERT INTO tasks (id, title, description, completed, created_date, due_date, category_id, priority, display_order) "
                 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

    for (const Task& task : tasks) {
        query.bindValue(0, task.id());
        query.bindValue(1, task.title());
        query.bindValue(2, task.description());
        query.bindValue(3, task.isCompleted() ? 1 : 0);
        query.bindValue(4, task.createdDate().toString(Qt::ISODate));
        query.bindValue(5, task.dueDate().isValid() ? task.dueDate().toString(Qt::ISODate) : "");
        query.bindValue(6, task.categoryId());
        query.bindValue(7, task.priority());
        query.bindValue(8, task.displayOrder());

        if (!query.exec()) {
            m_database.rollback();
            qWarning() << "Failed to save task:" << query.lastError().text();
            return false;
        }
    }

    return m_database.commit();
}

/**
 * @brief Load all tasks
 * 
 * Retrieves all tasks from the database.
 * 
 * @return QList<Task> List of tasks retrieved from the database
 */
QList<Task> DatabaseManager::loadTasks()
{
    QList<Task> tasks;

    if (!m_initialized) {
        return tasks;
    }

    QSqlQuery query("SELECT id, title, description, completed, created_date, due_date, category_id, priority, display_order FROM tasks");

    while (query.next()) {
        Task task;
        task.setId(query.value(0).toString());
        task.setTitle(query.value(1).toString());
        task.setDescription(query.value(2).toString());
        task.setCompleted(query.value(3).toBool());
        task.setCreatedDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));

        QString dueDateStr = query.value(5).toString();
        if (!dueDateStr.isEmpty()) {
            task.setDueDate(QDateTime::fromString(dueDateStr, Qt::ISODate));
        }

        task.setCategoryId(query.value(6).toString());
        task.setPriority(query.value(7).toInt());
        task.setDisplayOrder(query.value(8).toInt());

        tasks.append(task);
    }

    return tasks;
}

/**
 * @brief Save a single task
 * 
 * Saves a single task to the database. If a task with the same ID already exists,
 * it will be replaced with the new task.
 * 
 * @param task The Task object to save
 * @return bool True if the task was saved successfully, false otherwise
 */
bool DatabaseManager::saveTask(const Task& task)
{
    if (!m_initialized) {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO tasks (id, title, description, completed, created_date, due_date, category_id, priority) "
                 "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    query.bindValue(0, task.id());
    query.bindValue(1, task.title());
    query.bindValue(2, task.description());
    query.bindValue(3, task.isCompleted() ? 1 : 0);
    query.bindValue(4, task.createdDate().toString(Qt::ISODate));
    query.bindValue(5, task.dueDate().isValid() ? task.dueDate().toString(Qt::ISODate) : "");
    query.bindValue(6, task.categoryId());
    query.bindValue(7, task.priority());

    if (!query.exec()) {
        qWarning() << "Failed to save task:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Delete a task
 * 
 * Deletes a task from the database by its ID.
 * 
 * @param id The ID of the task to delete
 * @return bool True if the task was deleted successfully, false otherwise
 */
bool DatabaseManager::deleteTask(const QString& id)
{
    if (!m_initialized) {
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM tasks WHERE id = ?");
    query.bindValue(0, id);

    if (!query.exec()) {
        qWarning() << "Failed to delete task:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Save multiple categories
 * 
 * Saves a list of categories to the database in a single transaction.
 * This method deletes all existing categories and replaces them with the new list.
 * 
 * @param categories List of Category objects to save
 * @return bool True if all categories were saved successfully, false otherwise
 */
bool DatabaseManager::saveCategories(const QList<Category>& categories)
{
    if (!m_initialized) {
        return false;
    }

    // Start a transaction for better performance
    m_database.transaction();

    // Clear existing categories
    QSqlQuery clearQuery;
    if (!clearQuery.exec("DELETE FROM categories")) {
        m_database.rollback();
        return false;
    }

    // Insert all categories
    QSqlQuery query;
    query.prepare("INSERT INTO categories (id, name, color, is_default) VALUES (?, ?, ?, ?)");

    for (const Category& category : categories) {
        query.bindValue(0, category.id());
        query.bindValue(1, category.name());
        query.bindValue(2, category.color().name());
        query.bindValue(3, category.isDefault() ? 1 : 0);

        if (!query.exec()) {
            m_database.rollback();
            qWarning() << "Failed to save category:" << query.lastError().text();
            return false;
        }
    }

    return m_database.commit();
}

/**
 * @brief Load all categories
 * 
 * Retrieves all categories from the database.
 * 
 * @return QList<Category> List of categories retrieved from the database
 */
QList<Category> DatabaseManager::loadCategories()
{
    QList<Category> categories;

    if (!m_initialized) {
        return categories;
    }

    QSqlQuery query("SELECT id, name, color, is_default FROM categories");

    while (query.next()) {
        Category category;
        category.setId(query.value(0).toString());
        category.setName(query.value(1).toString());
        category.setColor(QColor(query.value(2).toString()));
        category.setDefault(query.value(3).toBool());

        categories.append(category);
    }

    return categories;
}

/**
 * @brief Save a single category
 * 
 * Saves a single category to the database. If a category with the same ID already exists,
 * it will be replaced with the new category.
 * 
 * @param category The Category object to save
 * @return bool True if the category was saved successfully, false otherwise
 */
bool DatabaseManager::saveCategory(const Category& category)
{
    qDebug() << "Saving category:" << category.name();
    if (!m_initialized) {
        return false;
    }
    qDebug() << "INITIALIZED";

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO categories (id, name, color, is_default) VALUES (?, ?, ?, ?)");

    query.bindValue(0, category.id());
    query.bindValue(1, category.name());
    query.bindValue(2, category.color().name());
    query.bindValue(3, category.isDefault() ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Failed to save category:" << query.lastError().text();
        return false;
    }

    return true;
}

/**
 * @brief Delete a category
 * 
 * Deletes a category from the database by its ID.
 * 
 * @param id The ID of the category to delete
 * @return bool True if the category was deleted successfully, false otherwise
 */
bool DatabaseManager::deleteCategory(const QString& id)
{
    if (!m_initialized) {
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM categories WHERE id = ?");
    query.bindValue(0, id);

    if (!query.exec()) {
        qWarning() << "Failed to delete category:" << query.lastError().text();
        return false;
    }

    return true;
}
