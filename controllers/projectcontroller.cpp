/**
 * @file projectcontroller.cpp
 * @brief Implementation of the ProjectController class
 * 
 * The ProjectController manages all project-related operations in the application.
 * It handles the creation, retrieval, updating, and deletion of projects,
 * while also acting as an intermediary between the UI and the project data model.
 * This controller implements a singleton pattern to ensure a single point of access.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "projectcontroller.h"
#include "../services/databasemanager.h"
#include <QDebug>

// Initialize static instance pointer
ProjectController* ProjectController::s_instance = nullptr;

/**
 * @brief Constructor for ProjectController
 *
 * Initializes the controller with the provided project model and sets up
 * signal connections to propagate model changes.
 *
 * @param model The ProjectModel to be managed by this controller
 * @param parent The parent QObject (optional)
 */
ProjectController::ProjectController(ProjectModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
    // Connect model signals to controller signals
    connect(m_model, &QAbstractItemModel::dataChanged,
            this, &ProjectController::projectsChanged);
    connect(m_model, &QAbstractItemModel::rowsInserted,
            this, &ProjectController::projectsChanged);
    connect(m_model, &QAbstractItemModel::rowsRemoved,
            this, &ProjectController::projectsChanged);
    connect(m_model, &QAbstractItemModel::modelReset,
            this, &ProjectController::projectsChanged);
}

/**
 * @brief Destructor for ProjectController
 *
 * Saves all projects to the database before destruction.
 */
ProjectController::~ProjectController()
{
    // Save projects before destruction
    saveProjects();
}

/**
 * @brief Get the singleton instance of ProjectController
 *
 * Creates a new instance if one doesn't exist yet.
 * 
 * @param model Optional ProjectModel to use; if null, a new model is created
 * @return Reference to the singleton instance
 */
ProjectController& ProjectController::instance(ProjectModel *model)
{
    if (!s_instance) {
        if (!model) {
            model = new ProjectModel();
        }
        s_instance = new ProjectController(model);
    }
    return *s_instance;
}

/**
 * @brief Cleanup the singleton instance
 * 
 * Deletes the singleton instance and sets it to nullptr.
 * Useful for testing and application shutdown.
 */
void ProjectController::cleanup()
{
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

/**
 * @brief Get the ProjectModel managed by this controller
 *
 * @return Pointer to the ProjectModel
 */
ProjectModel* ProjectController::model() const
{
    return m_model;
}

/**
 * @brief Add a new project with the specified properties
 *
 * Creates a new Project object and adds it to both the model and database.
 *
 * @param name The name of the project
 * @param color The color associated with the project
 * @param description The project description
 * @return true if the project was successfully added, false otherwise
 */
bool ProjectController::addProject(const QString &name, const QColor &color, const QString &description)
{
    // Validate input
    if (name.isEmpty()) {
        qWarning() << "Cannot add project with empty name";
        return false;
    }
    
    // Create new project
    Project project(name, color, description);
    
    // Add to model
    m_model->addProject(project);
    
    // Save to database
    bool success = DatabaseManager::instance().saveProject(project);
    
    if (success) {
        emit projectAdded(project);
        qDebug() << "Added project:" << project.name();
    } else {
        qWarning() << "Failed to add project to database";
    }
    
    return success;
}

/**
 * @brief Add an existing Project object
 *
 * Adds the provided Project to both the model and database.
 *
 * @param project The Project object to add
 * @return true if the project was successfully added, false otherwise
 */
bool ProjectController::addProject(const Project &project)
{
    // Validate input
    if (project.name().isEmpty()) {
        qWarning() << "Cannot add project with empty name";
        return false;
    }
    
    // Add to model
    m_model->addProject(project);
    
    // Save to database
    bool success = DatabaseManager::instance().saveProject(project);
    
    if (success) {
        emit projectAdded(project);
        qDebug() << "Added project:" << project.name();
    } else {
        qWarning() << "Failed to add project to database";
    }
    
    return success;
}

/**
 * @brief Update an existing project with new properties
 *
 * Finds the project by ID and updates its properties in both model and database.
 *
 * @param id The ID of the project to update
 * @param name The new name
 * @param color The new color
 * @param description The new description
 * @param isActive The new active status
 * @return true if the project was successfully updated, false otherwise
 */
bool ProjectController::updateProject(const QString &id, const QString &name, const QColor &color, 
                                    const QString &description, bool isActive)
{
    // Validate input
    if (name.isEmpty()) {
        qWarning() << "Cannot update project with empty name";
        return false;
    }
    
    // Get existing project
    Project project = getProject(id);
    if (project.id().isEmpty()) {
        qWarning() << "Project not found:" << id;
        return false;
    }
    
    // Update properties
    project.setName(name);
    project.setColor(color);
    project.setDescription(description);
    project.setActive(isActive);
    
    // Update in model
    bool modelSuccess = m_model->updateProject(project);
    
    if (!modelSuccess) {
        qWarning() << "Failed to update project in model:" << id;
        return false;
    }
    
    // Save to database
    bool dbSuccess = DatabaseManager::instance().saveProject(project);
    
    if (dbSuccess) {
        emit projectUpdated(project);
        qDebug() << "Updated project:" << id;
    } else {
        qWarning() << "Failed to update project in database";
    }
    
    return dbSuccess;
}

/**
 * @brief Update an existing project with a Project object
 *
 * Updates the project in both model and database using the provided Project object.
 *
 * @param project The Project object with updated properties
 * @return true if the project was successfully updated, false otherwise
 */
bool ProjectController::updateProject(const Project &project)
{
    // Validate input
    if (project.name().isEmpty()) {
        qWarning() << "Cannot update project with empty name";
        return false;
    }
    
    // Check if project exists
    Project existingProject = getProject(project.id());
    if (existingProject.id().isEmpty()) {
        qWarning() << "Project not found:" << project.id();
        return false;
    }
    
    // Update in model
    bool modelSuccess = m_model->updateProject(project);
    
    if (!modelSuccess) {
        qWarning() << "Failed to update project in model:" << project.id();
        return false;
    }
    
    // Save to database
    bool dbSuccess = DatabaseManager::instance().saveProject(project);
    
    if (dbSuccess) {
        emit projectUpdated(project);
        qDebug() << "Updated project:" << project.id();
    } else {
        qWarning() << "Failed to update project in database";
    }
    
    return dbSuccess;
}

/**
 * @brief Delete a project by ID
 *
 * Removes the project from both model and database.
 *
 * @param id The ID of the project to delete
 * @return true if the project was successfully deleted, false otherwise
 */
bool ProjectController::deleteProject(const QString &id)
{
    // Get the project before removing it (for the signal)
    Project project = getProject(id);
    
    // Remove from model
    bool modelSuccess = m_model->removeProject(id);
    
    if (!modelSuccess) {
        qWarning() << "Failed to remove project from model:" << id;
        return false;
    }
    
    // Delete from database
    bool dbSuccess = DatabaseManager::instance().deleteProject(id);
    
    if (dbSuccess) {
        emit projectDeleted(id);
        qDebug() << "Deleted project:" << id;
    } else {
        qWarning() << "Failed to delete project from database";
    }
    
    return dbSuccess;
}

/**
 * @brief Get a project by ID
 *
 * @param id The ID of the project to retrieve
 * @return The Project object if found, or an empty Project otherwise
 */
Project ProjectController::getProject(const QString &id) const
{
    return m_model->getProject(id);
}

/**
 * @brief Get all projects
 *
 * @return A list of all Project objects
 */
QList<Project> ProjectController::getProjects() const
{
    return m_model->getProjects();
}

/**
 * @brief Get only active projects
 *
 * @return A list of active Project objects
 */
QList<Project> ProjectController::getActiveProjects() const
{
    return m_model->getActiveProjects();
}

/**
 * @brief Load projects from the database
 *
 * Loads all projects from the database and updates the model.
 * Creates default projects if none exist.
 *
 * @return true if projects were loaded successfully, false otherwise
 */
bool ProjectController::loadProjects()
{
    QList<Project> projects = DatabaseManager::instance().loadProjects();
    m_model->setProjects(projects);
    foreach (Project var, projects) {
        qDebug()<<var.name() << var.isActive();
    }
    
    // Create default projects if none exist
    if (projects.isEmpty()) {
        createDefaultProjects();
    }
    
    qDebug() << "Loaded" << projects.size() << "projects from database";
    return true;
}

/**
 * @brief Save all projects to the database
 *
 * @return true if projects were saved successfully, false otherwise
 */
bool ProjectController::saveProjects()
{
    QList<Project> projects = m_model->getProjects();
    bool success = DatabaseManager::instance().saveProjects(projects);
    
    if (success) {
        qDebug() << "Saved" << projects.size() << "projects to database";
    } else {
        qWarning() << "Failed to save projects to database";
    }
    
    return success;
}

/**
 * @brief Create default projects
 *
 * Creates a set of default projects with predefined names and colors.
 * This is called when no projects exist in the database on first run.
 *
 * @return true if default projects were created successfully, false otherwise
 */
bool ProjectController::createDefaultProjects()
{
    // Add some default projects with different colors
    addProject("Personal", QColor(52, 152, 219), "Personal activities and tasks");
    addProject("Work", QColor(231, 76, 60), "Work-related activities and tasks");
    addProject("Learning", QColor(46, 204, 113), "Educational activities and learning");
    addProject("Health", QColor(155, 89, 182), "Health and fitness activities");
    
    return true;
}

/**
 * @brief Get time entries for a specific project
 *
 * Retrieves all time entries associated with a project from the database.
 *
 * @param projectId The ID of the project
 * @return A list of TimeEntry objects for the specified project
 */
QList<TimeEntry> ProjectController::getTimeEntriesForProject(const QString &projectId) const
{
    // Query the database for time entries related to this project
    return DatabaseManager::instance().getTimeEntriesForProject(projectId);
}
