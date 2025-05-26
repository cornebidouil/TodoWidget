/**
 * @file projectcontroller.h
 * @brief Definition of the ProjectController class
 * 
 * This file defines the ProjectController class which manages project operations
 * in the application, including adding, updating, and deleting projects.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QObject>
#include "../models/projectmodel.h"
#include "../models/timeentrymodel.h"

/**
 * @class ProjectController
 * @brief Controller for project operations
 * 
 * The ProjectController class manages all project-related operations in the application.
 * It provides methods for creating, updating, and deleting projects, as well as
 * persisting project data to the database.
 */
class ProjectController : public QObject {
    Q_OBJECT
    
public:
    /**
     * @brief Get the singleton instance
     * @param model Optional project model to use (only used on first call)
     * @return ProjectController& Reference to the singleton instance
     */
    static ProjectController& instance(ProjectModel *model = nullptr);
    
    /**
     * @brief Destructor
     */
    ~ProjectController();
    
    /**
     * @brief Cleanup the singleton instance
     * 
     * Deletes the singleton instance and sets it to nullptr.
     * Useful for testing and application shutdown.
     */
    static void cleanup();
    
    /**
     * @brief Get the project model
     * @return ProjectModel* Pointer to the project model
     */
    ProjectModel* model() const;
    
    /**
     * @brief Add a new project
     * 
     * Creates and adds a new project with the specified properties.
     * 
     * @param name The name of the project
     * @param color The color of the project
     * @param description The description of the project
     * @return bool True if the project was added successfully, false otherwise
     */
    bool addProject(const QString &name, const QColor &color, const QString &description = QString());
    
    /**
     * @brief Add a new project
     * 
     * Adds a new project from a Project object.
     * 
     * @param project The project to add
     * @return bool True if the project was added successfully, false otherwise
     */
    bool addProject(const Project &project);
    
    /**
     * @brief Update a project
     * 
     * Updates an existing project with the specified properties.
     * 
     * @param id The ID of the project to update
     * @param name The new name of the project
     * @param color The new color of the project
     * @param description The new description of the project
     * @param isActive Whether the project is active
     * @return bool True if the project was updated successfully, false otherwise
     */
    bool updateProject(const QString &id, const QString &name, const QColor &color, 
                      const QString &description, bool isActive);
                      
    /**
     * @brief Update a project
     * 
     * Updates an existing project from a Project object.
     * 
     * @param project The project to update
     * @return bool True if the project was updated successfully, false otherwise
     */
    bool updateProject(const Project &project);
    
    /**
     * @brief Delete a project
     * 
     * Removes a project from the model and the database.
     * 
     * @param id The ID of the project to delete
     * @return bool True if the project was deleted successfully, false otherwise
     */
    bool deleteProject(const QString &id);
    
    /**
     * @brief Get a project by ID
     * 
     * Retrieves a project from the model by its ID.
     * 
     * @param id The ID of the project to get
     * @return Project The project with the specified ID, or an empty project if not found
     */
    Project getProject(const QString &id) const;
    
    /**
     * @brief Get all projects
     * 
     * Retrieves all projects from the model.
     * 
     * @return QList<Project> All projects in the model
     */
    QList<Project> getProjects() const;
    
    /**
     * @brief Get active projects
     * 
     * Retrieves all active projects from the model.
     * 
     * @return QList<Project> All active projects in the model
     */
    QList<Project> getActiveProjects() const;
    
    /**
     * @brief Load all projects from the database
     * 
     * Retrieves all projects from the database and updates the model.
     * 
     * @return bool True if projects were loaded successfully, false otherwise
     */
    bool loadProjects();
    
    /**
     * @brief Save all projects to the database
     * 
     * Persists all projects in the model to the database.
     * 
     * @return bool True if projects were saved successfully, false otherwise
     */
    bool saveProjects();
    
    /**
     * @brief Create default projects
     * 
     * Creates a set of default projects if none exist.
     * 
     * @return bool True if default projects were created successfully, false otherwise
     */
    bool createDefaultProjects();

    /**
     * @brief Get time entries for a project
     * 
     * Retrieves all time entries associated with the specified project.
     * 
     * @param projectId The ID of the project to get time entries for
     * @return QList<TimeEntry> Time entries for the project
     */
    QList<TimeEntry> getTimeEntriesForProject(const QString &projectId) const;

signals:
    /**
     * @brief Signal emitted when a project is added
     * @param project The added project
     */
    void projectAdded(const Project &project);
    
    /**
     * @brief Signal emitted when a project is updated
     * @param project The updated project
     */
    void projectUpdated(const Project &project);
    
    /**
     * @brief Signal emitted when a project is deleted
     * @param id The ID of the deleted project
     */
    void projectDeleted(const QString &id);
    
    /**
     * @brief Signal emitted when the list of projects changes
     */
    void projectsChanged();

private:
    /**
     * @brief Private constructor to enforce singleton pattern
     * @param model The project model to manage
     * @param parent Optional parent QObject
     */
    explicit ProjectController(ProjectModel *model, QObject *parent = nullptr);

    /**
     * @brief Private copy constructor to enforce singleton pattern
     */
    ProjectController(const ProjectController&) = delete;
    
    /**
     * @brief Private assignment operator to enforce singleton pattern
     */
    ProjectController& operator=(const ProjectController&) = delete;

    ProjectModel *m_model;  ///< The project model
    static ProjectController *s_instance;  ///< Singleton instance
};
