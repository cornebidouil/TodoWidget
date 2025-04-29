/**
 * @file importexportservice.h
 * @brief Definition of the ImportExportService class
 * 
 * This file defines the ImportExportService class which provides functionality
 * for importing and exporting tasks and categories to and from various file formats.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include "../models/task.h"
#include "../models/category.h"

/**
 * @class ImportExportService
 * @brief Service for importing and exporting application data
 * 
 * The ImportExportService class provides methods for importing and exporting
 * tasks and categories in various formats (JSON, CSV). This allows users to
 * backup their data, transfer it between devices, or integrate with other
 * applications.
 */
class ImportExportService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new ImportExportService.
     * 
     * @param parent Optional parent QObject
     */
    explicit ImportExportService(QObject* parent = nullptr);

    // Export functions
    /**
     * @brief Export tasks to a JSON file
     * 
     * Serializes a list of tasks to JSON format and saves it to a file.
     * 
     * @param filePath Path to the output file
     * @param tasks List of tasks to export
     * @return bool True if export was successful, false otherwise
     */
    bool exportTasksToJson(const QString& filePath, const QList<Task>& tasks);
    
    /**
     * @brief Export tasks to a CSV file
     * 
     * Serializes a list of tasks to CSV format and saves it to a file.
     * 
     * @param filePath Path to the output file
     * @param tasks List of tasks to export
     * @return bool True if export was successful, false otherwise
     */
    bool exportTasksToCsv(const QString& filePath, const QList<Task>& tasks);
    
    /**
     * @brief Export categories to a JSON file
     * 
     * Serializes a list of categories to JSON format and saves it to a file.
     * 
     * @param filePath Path to the output file
     * @param categories List of categories to export
     * @return bool True if export was successful, false otherwise
     */
    bool exportCategoriesToJson(const QString& filePath, const QList<Category>& categories);

    // Import functions
    /**
     * @brief Import tasks from a JSON file
     * 
     * Reads a JSON file and deserializes it into a list of tasks.
     * 
     * @param filePath Path to the input file
     * @param ok Optional pointer to a bool that will be set to true if import was successful
     * @return QList<Task> List of imported tasks
     */
    QList<Task> importTasksFromJson(const QString& filePath, bool* ok = nullptr);
    
    /**
     * @brief Import tasks from a CSV file
     * 
     * Reads a CSV file and deserializes it into a list of tasks.
     * 
     * @param filePath Path to the input file
     * @param ok Optional pointer to a bool that will be set to true if import was successful
     * @return QList<Task> List of imported tasks
     */
    QList<Task> importTasksFromCsv(const QString& filePath, bool* ok = nullptr);
    
    /**
     * @brief Import categories from a JSON file
     * 
     * Reads a JSON file and deserializes it into a list of categories.
     * 
     * @param filePath Path to the input file
     * @param ok Optional pointer to a bool that will be set to true if import was successful
     * @return QList<Category> List of imported categories
     */
    QList<Category> importCategoriesFromJson(const QString& filePath, bool* ok = nullptr);

private:
    /**
     * @brief Convert a task to a CSV line
     * 
     * Helper method that serializes a single task to a CSV-formatted string.
     * 
     * @param task The task to convert
     * @return QString CSV-formatted representation of the task
     */
    QString taskToCsvLine(const Task& task);
    
    /**
     * @brief Create a task from a CSV line
     * 
     * Helper method that deserializes a single task from a CSV-formatted string.
     * 
     * @param line CSV-formatted string representing a task
     * @param ok Optional pointer to a bool that will be set to true if parsing was successful
     * @return Task The parsed task
     */
    Task taskFromCsvLine(const QString& line, bool* ok = nullptr);
};
