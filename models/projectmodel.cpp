/**
 * @file projectmodel.cpp
 * @brief Implementation of the ProjectModel class
 * 
 * The ProjectModel class manages a collection of Project objects and provides
 * an interface for the Qt Model/View framework. It inherits from QAbstractListModel
 * and implements the necessary methods for data access, modification, and role-based
 * data retrieval.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include <QDebug>

#include "projectmodel.h"

/**
 * @brief Constructor for ProjectModel
 * 
 * @param parent The parent QObject
 */
ProjectModel::ProjectModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

/**
 * @brief Destructor for ProjectModel
 */
ProjectModel::~ProjectModel()
{
}

/**
 * @brief Get the number of rows in the model
 * 
 * Implements QAbstractListModel::rowCount() to return the number of projects.
 * 
 * @param parent The parent model index (unused in list models)
 * @return The number of projects in the model
 */
int ProjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_projects.count();
}

/**
 * @brief Get data for a specific model index and role
 * 
 * Implements QAbstractListModel::data() to provide access to project data
 * based on the requested role.
 * 
 * @param index The model index to get data for
 * @param role The data role (DisplayRole, IdRole, NameRole, etc.)
 * @return The requested data as a QVariant
 */
QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_projects.size()) {
        return QVariant();
    }
    
    const Project &project = m_projects.at(index.row());
    
    switch (role) {
        case Qt::DisplayRole:
        case NameRole:
            return project.name();
        case IdRole:
            return project.id();
        case ColorRole:
            return project.color();
        case DescriptionRole:
            return project.description();
        case IsActiveRole:
            return project.isActive();
        default:
            return QVariant();
    }
}

/**
 * @brief Set data for a specific model index and role
 * 
 * Implements QAbstractListModel::setData() to modify project properties
 * based on the specified role.
 * 
 * @param index The model index to set data for
 * @param value The new value to set
 * @param role The data role (NameRole, ColorRole, etc.)
 * @return true if the data was successfully set, false otherwise
 */
bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_projects.size()) {
        return false;
    }
    
    Project &project = m_projects[index.row()];
    
    bool success = false;
    
    switch (role) {
        case NameRole:
            project.setName(value.toString());
            success = true;
            break;
        case ColorRole:
            project.setColor(value.value<QColor>());
            success = true;
            break;
        case DescriptionRole:
            project.setDescription(value.toString());
            success = true;
            break;
        case IsActiveRole:
            project.setActive(value.toBool());
            success = true;
            break;
        default:
            break;
    }
    
    if (success) {
        emit dataChanged(index, index, {role});
        emit projectsChanged();
    }
    
    return success;
}

/**
 * @brief Get item flags for a model index
 * 
 * Implements QAbstractListModel::flags() to specify item capabilities.
 * 
 * @param index The model index to get flags for
 * @return The item flags (ItemIsEnabled, ItemIsSelectable, ItemIsEditable)
 */
Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

/**
 * @brief Get the role names for the model
 * 
 * Implements QAbstractListModel::roleNames() to provide role names for QML integration.
 * 
 * @return A hash mapping role values to role names
 */
QHash<int, QByteArray> ProjectModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[ColorRole] = "color";
    roles[DescriptionRole] = "description";
    roles[IsActiveRole] = "isActive";
    return roles;
}

/**
 * @brief Add a project to the model
 * 
 * Adds a project to the end of the list and notifies views of the change.
 * 
 * @param project The Project object to add
 */
void ProjectModel::addProject(const Project &project)
{
    beginInsertRows(QModelIndex(), m_projects.size(), m_projects.size());
    m_projects.append(project);
    endInsertRows();
    
    emit projectsChanged();
}

/**
 * @brief Remove a project from the model
 * 
 * Removes the project with the specified ID and notifies views of the change.
 * 
 * @param id The ID of the project to remove
 * @return true if the project was found and removed, false otherwise
 */
bool ProjectModel::removeProject(const QString &id)
{
    int index = findIndexById(id);
    if (index == -1) {
        return false;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    m_projects.removeAt(index);
    endRemoveRows();
    
    emit projectsChanged();
    return true;
}

/**
 * @brief Get a project by ID
 * 
 * @param id The ID of the project to retrieve
 * @return The Project object if found, or an empty Project otherwise
 */
Project ProjectModel::getProject(const QString &id) const
{
    int index = findIndexById(id);
    if (index == -1) {
        return Project();
    }
    
    return m_projects.at(index);
}

/**
 * @brief Get all projects
 * 
 * @return A list of all Project objects in the model
 */
QList<Project> ProjectModel::getProjects() const
{
    return m_projects;
}

/**
 * @brief Get only active projects
 * 
 * Filters the projects list to include only active projects.
 * 
 * @return A list of active Project objects
 */
QList<Project> ProjectModel::getActiveProjects() const
{
    qDebug()<<"getActiveProjects()" << m_projects.size();
    QList<Project> activeProjects;
    for (const Project &project : m_projects) {
        qDebug() << "\t" <<project.name() << project.isActive();
        if (project.isActive()) {
            activeProjects.append(project);
        }
    }
    qDebug() << "getActiveProjects() - Active projects found:" << activeProjects.size();
    return activeProjects;
}

/**
 * @brief Set the list of projects
 * 
 * Replaces the entire list of projects and notifies views of the change.
 * 
 * @param projects The new list of Project objects
 */
void ProjectModel::setProjects(const QList<Project> &projects)
{
    beginResetModel();
    m_projects = projects;
    endResetModel();
    
    qDebug() << "ProjectModel::setProjects - Projects after setting:";
    for (const Project &project : m_projects) {
        qDebug() << "\t" << project.name() << project.isActive();
    }
    qDebug()<< "Projects transmitted";
    for (const Project &project : projects) {
        qDebug() << "\t" << project.name() << project.isActive();
    }

    emit projectsChanged();
}

/**
 * @brief Update an existing project
 * 
 * Finds a project by ID and updates it with the provided Project object.
 * 
 * @param project The Project object with updated properties
 * @return true if the project was found and updated, false otherwise
 */
bool ProjectModel::updateProject(const Project &project)
{
    int index = findIndexById(project.id());
    if (index == -1) {
        return false;
    }
    
    m_projects[index] = project;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
    
    emit projectsChanged();
    return true;
}

/**
 * @brief Refresh the model views
 * 
 * Forces views to refresh by emitting dataChanged for all rows.
 * This is useful when project data has been modified externally.
 */
void ProjectModel::refresh()
{
    // Simply emit dataChanged for all rows to force views to refresh
    if (!m_projects.isEmpty()) {
        emit dataChanged(
            createIndex(0, 0),
            createIndex(m_projects.size() - 1, 0),
            {Qt::DisplayRole, NameRole, ColorRole, DescriptionRole, IsActiveRole}
        );
    }
    
    // Also emit projectsChanged for any custom handling
    emit projectsChanged();
}

/**
 * @brief Find the index of a project by ID
 * 
 * Helper method to find the index of a project in the list by its ID.
 * 
 * @param id The ID of the project to find
 * @return The index of the project, or -1 if not found
 */
int ProjectModel::findIndexById(const QString &id) const
{
    for (int i = 0; i < m_projects.size(); ++i) {
        if (m_projects.at(i).id() == id) {
            return i;
        }
    }
    
    return -1;
}
