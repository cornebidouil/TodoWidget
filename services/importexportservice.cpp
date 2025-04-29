/**
 * @file importexportservice.cpp
 * @brief Implementation of the ImportExportService class
 * 
 * This file implements the ImportExportService class which provides functionality
 * for importing and exporting tasks and categories in various file formats (JSON, CSV).
 * It handles serialization and deserialization of application data for backup,
 * migration, or sharing purposes.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "importexportservice.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QDebug>

/**
 * @brief Constructor
 * 
 * Creates a new ImportExportService instance.
 * 
 * @param parent Optional parent QObject
 */
ImportExportService::ImportExportService(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief Export tasks to JSON format
 * 
 * Serializes a list of tasks to a JSON file.
 * The file will contain a JSON array of task objects.
 * 
 * @param filePath Path to the output file
 * @param tasks List of Task objects to export
 * @return bool True if export was successful, false otherwise
 */
bool ImportExportService::exportTasksToJson(const QString& filePath, const QList<Task>& tasks)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QJsonArray taskArray;
    for (const Task& task : tasks) {
        taskArray.append(task.toJson());
    }

    QJsonDocument doc(taskArray);
    file.write(doc.toJson());
    file.close();

    return true;
}

/**
 * @brief Export tasks to CSV format
 * 
 * Serializes a list of tasks to a CSV file.
 * The file will include a header row and a row for each task.
 * 
 * @param filePath Path to the output file
 * @param tasks List of Task objects to export
 * @return bool True if export was successful, false otherwise
 */
bool ImportExportService::exportTasksToCsv(const QString& filePath, const QList<Task>& tasks)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QTextStream out(&file);

    // Write header row
    out << "ID,Title,Description,Completed,CreatedDate,DueDate,CategoryID,Priority\n";

    // Write task rows
    for (const Task& task : tasks) {
        out << taskToCsvLine(task) << "\n";
    }

    file.close();
    return true;
}

/**
 * @brief Export categories to JSON format
 * 
 * Serializes a list of categories to a JSON file.
 * The file will contain a JSON array of category objects.
 * 
 * @param filePath Path to the output file
 * @param categories List of Category objects to export
 * @return bool True if export was successful, false otherwise
 */
bool ImportExportService::exportCategoriesToJson(const QString& filePath, const QList<Category>& categories)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QJsonArray categoryArray;
    for (const Category& category : categories) {
        categoryArray.append(category.toJson());
    }

    QJsonDocument doc(categoryArray);
    file.write(doc.toJson());
    file.close();

    return true;
}

/**
 * @brief Import tasks from JSON format
 * 
 * Deserializes tasks from a JSON file.
 * The file should contain a JSON array of task objects.
 * 
 * @param filePath Path to the input file
 * @param ok Optional pointer to boolean that will be set to true if import was successful
 * @return QList<Task> List of imported Task objects
 */
QList<Task> ImportExportService::importTasksFromJson(const QString& filePath, bool* ok)
{
    QList<Task> tasks;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << filePath;
        if (ok) *ok = false;
        return tasks;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        if (ok) *ok = false;
        return tasks;
    }

    QJsonArray taskArray = doc.array();
    for (const QJsonValue& value : taskArray) {
        if (value.isObject()) {
            tasks.append(Task::fromJson(value.toObject()));
        }
    }

    if (ok) *ok = true;
    return tasks;
}

/**
 * @brief Import tasks from CSV format
 * 
 * Deserializes tasks from a CSV file.
 * The file should include a header row and a row for each task.
 * 
 * @param filePath Path to the input file
 * @param ok Optional pointer to boolean that will be set to true if import was successful
 * @return QList<Task> List of imported Task objects
 */
QList<Task> ImportExportService::importTasksFromCsv(const QString& filePath, bool* ok)
{
    QList<Task> tasks;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for reading:" << filePath;
        if (ok) *ok = false;
        return tasks;
    }

    QTextStream in(&file);

    // Skip header row
    if (!in.atEnd()) {
        in.readLine();
    }

    // Read task rows
    while (!in.atEnd()) {
        QString line = in.readLine();
        bool lineOk = true;
        Task task = taskFromCsvLine(line, &lineOk);

        if (lineOk) {
            tasks.append(task);
        }
    }

    file.close();
    if (ok) *ok = true;
    return tasks;
}

/**
 * @brief Import categories from JSON format
 * 
 * Deserializes categories from a JSON file.
 * The file should contain a JSON array of category objects.
 * 
 * @param filePath Path to the input file
 * @param ok Optional pointer to boolean that will be set to true if import was successful
 * @return QList<Category> List of imported Category objects
 */
QList<Category> ImportExportService::importCategoriesFromJson(const QString& filePath, bool* ok)
{
    QList<Category> categories;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << filePath;
        if (ok) *ok = false;
        return categories;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        if (ok) *ok = false;
        return categories;
    }

    QJsonArray categoryArray = doc.array();
    for (const QJsonValue& value : categoryArray) {
        if (value.isObject()) {
            categories.append(Category::fromJson(value.toObject()));
        }
    }

    if (ok) *ok = true;
    return categories;
}

/**
 * @brief Convert a task to a CSV line
 * 
 * Helper method that serializes a Task object to a CSV line string.
 * Handles proper escaping of fields and formats dates according to ISO standard.
 * 
 * @param task The Task object to convert
 * @return QString The formatted CSV line
 */
QString ImportExportService::taskToCsvLine(const Task& task)
{
    // Format: ID,Title,Description,Completed,CreatedDate,DueDate,CategoryID,Priority
    QString line;

    // Escape quotes and handle commas in title and description
    QString escapedTitle = task.title();
    escapedTitle.replace("\"", "\"\"");  // Double quotes to escape quotes
    if (escapedTitle.contains(',')) {
        escapedTitle = "\"" + escapedTitle + "\"";  // Quote fields with commas
    }

    QString escapedDesc = task.description();
    escapedDesc.replace("\"", "\"\"");
    if (escapedDesc.contains(',')) {
        escapedDesc = "\"" + escapedDesc + "\"";
    }

    // Build the CSV line with each field
    line += task.id() + ",";
    line += escapedTitle + ",";
    line += escapedDesc + ",";
    line += QString(task.isCompleted() ? "1" : "0") + ",";
    line += task.createdDate().toString(Qt::ISODate) + ",";
    line += (task.dueDate().isValid() ? task.dueDate().toString(Qt::ISODate) : "") + ",";
    line += task.categoryId() + ",";
    line += QString::number(task.priority());

    return line;
}

/**
 * @brief Convert a CSV line to a task
 * 
 * Helper method that deserializes a Task object from a CSV line string.
 * Handles quoted fields and proper parsing of dates.
 * 
 * @param line The CSV line to parse
 * @param ok Optional pointer to boolean that will be set to true if parsing was successful
 * @return Task The parsed Task object
 */
Task ImportExportService::taskFromCsvLine(const QString& line, bool* ok)
{
    Task task;

    if (ok) *ok = false;

    // Parse CSV line with proper handling of quoted fields
    QStringList fields;
    bool inQuotes = false;
    QString field;

    // Custom CSV parser to handle quoted fields correctly
    for (int i = 0; i < line.length(); ++i) {
        QChar c = line.at(i);

        if (c == '\"') {
            if (i + 1 < line.length() && line.at(i + 1) == '\"') {
                // Handle escaped quote (two consecutive quotes)
                field += '\"';
                ++i; // Skip the next quote
            } else {
                // Toggle quote mode
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            // End of field (comma outside quotes)
            fields.append(field);
            field.clear();
        } else {
            // Normal character
            field += c;
        }
    }

    // Add the last field
    fields.append(field);

    // Validate field count
    if (fields.size() != 8) {
        qWarning() << "Invalid CSV line format, expected 8 fields but got" << fields.size();
        return task;
    }

    // Parse fields into a Task object
    task.setId(fields[0]);
    task.setTitle(fields[1]);
    task.setDescription(fields[2]);
    task.setCompleted(fields[3] == "1");
    task.setCreatedDate(QDateTime::fromString(fields[4], Qt::ISODate));

    if (!fields[5].isEmpty()) {
        task.setDueDate(QDateTime::fromString(fields[5], Qt::ISODate));
    }

    task.setCategoryId(fields[6]);
    task.setPriority(fields[7].toInt());

    if (ok) *ok = true;
    return task;
}
