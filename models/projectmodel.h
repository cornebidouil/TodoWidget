/**
 * @file projectmodel.h
 * @brief Definition of the ProjectModel class
 * 
 * This file defines the ProjectModel class which serves as a model for
 * a collection of Project objects in the application. It inherits from
 * QAbstractListModel to provide model/view integration with Qt widgets.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QAbstractListModel>
#include <QList>
#include "project.h"

/**
 * @class ProjectModel
 * @brief Model for projects
 * 
 * The ProjectModel class provides a model for working with collections of Project objects.
 * It implements the necessary methods from QAbstractListModel to support Qt's model/view architecture.
 */
class ProjectModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    /**
     * @brief Custom roles for project data
     */
    enum ProjectRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        ColorRole,
        DescriptionRole,
        IsActiveRole
    };
    
    /**
     * @brief Constructor
     * 
     * Creates a new project model.
     * 
     * @param parent Optional parent object
     */
    explicit ProjectModel(QObject *parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~ProjectModel();

    // QAbstractItemModel implementation
    
    /**
     * @brief Get the number of rows in the model
     * @param parent Parent model index (unused)
     * @return int Number of projects in the model
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    
    /**
     * @brief Get data for a specific index and role
     * @param index Model index to get data for
     * @param role Data role
     * @return QVariant The requested data
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    /**
     * @brief Set data for a specific index and role
     * @param index Model index to set data for
     * @param value The value to set
     * @param role Data role
     * @return bool True if data was set successfully, false otherwise
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    
    /**
     * @brief Get the item flags for a specific index
     * @param index Model index to get flags for
     * @return Qt::ItemFlags The item flags
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    /**
     * @brief Get the role names for use with QML
     * @return QHash<int, QByteArray> The role names
     */
    QHash<int, QByteArray> roleNames() const override;

    // Project management methods
    
    /**
     * @brief Add a project to the model
     * @param project The project to add
     */
    void addProject(const Project &project);
    
    /**
     * @brief Remove a project from the model
     * @param id The ID of the project to remove
     * @return bool True if the project was removed, false otherwise
     */
    bool removeProject(const QString &id);
    
    /**
     * @brief Get a project by ID
     * @param id The ID of the project to get
     * @return Project The project with the specified ID, or an empty project if not found
     */
    Project getProject(const QString &id) const;
    
    /**
     * @brief Get all projects
     * @return QList<Project> All projects in the model
     */
    QList<Project> getProjects() const;
    
    /**
     * @brief Get active projects
     * @return QList<Project> Active projects in the model
     */
    QList<Project> getActiveProjects() const;
    
    /**
     * @brief Set all projects
     * @param projects The projects to set
     */
    void setProjects(const QList<Project> &projects);
    
    /**
     * @brief Update a project
     * @param project The project to update
     * @return bool True if the project was updated, false otherwise
     */
    bool updateProject(const Project &project);

    /**
     * @brief Refresh the model's data
     * 
     * Forces the views to reload data by resetting the model.
     * This is useful when the model's data may have changed externally.
     */
    void refresh();

signals:
    /**
     * @brief Signal emitted when the list of projects changes
     */
    void projectsChanged();

private:
    QList<Project> m_projects;  ///< The list of projects
    
    /**
     * @brief Find the index of a project with the specified ID
     * @param id The ID to search for
     * @return int The index of the project, or -1 if not found
     */
    int findIndexById(const QString &id) const;
};
