/**
 * @file projectstab.h
 * @brief Definition of the ProjectsTab class
 * 
 * This file defines the ProjectsTab class which provides a tab interface
 * for managing projects in the settings dialog.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#pragma once

#include <QWidget>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../models/projectmodel.h"
#include "../controllers/projectcontroller.h"

/**
 * @class ProjectsTab
 * @brief Tab for project management in settings dialog
 * 
 * The ProjectsTab class provides a user interface for managing projects
 * within the settings dialog. It includes a list view of projects and
 * buttons for adding, editing, and deleting projects.
 */
class ProjectsTab : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * 
     * Creates a new ProjectsTab that uses the ProjectController singleton internally.
     * 
     * @param parent Optional parent widget
     */
    explicit ProjectsTab(QWidget *parent = nullptr);
    
    /**
     * @brief Refresh the projects list
     * 
     * Updates the projects display with current data.
     */
    void refreshProjects();

private slots:
    /**
     * @brief Handle add project button click
     * 
     * Opens the project editor dialog to add a new project.
     */
    void onAddProjectClicked();
    
    /**
     * @brief Handle edit project button click
     * 
     * Opens the project editor dialog to edit the selected project.
     */
    void onEditProjectClicked();
    
    /**
     * @brief Handle delete project button click
     * 
     * Deletes the selected project after confirmation.
     */
    void onDeleteProjectClicked();
    
    /**
     * @brief Handle project double click
     * 
     * Opens the project editor for the double-clicked project.
     * 
     * @param index Model index of the double-clicked project
     */
    void onProjectDoubleClicked(const QModelIndex &index);

private:
    /**
     * @brief Set up the user interface
     * 
     * Creates and arranges all UI elements.
     */
    void setupUi();
    
    /**
     * @brief Set up signal-slot connections
     * 
     * Connects UI elements to their corresponding slots.
     */
    void setupConnections();

    ProjectModel *m_projectModel;             ///< Pointer to the project model

    QListView *m_projectListView;             ///< List view for projects
    QPushButton *m_addButton;                 ///< Button for adding a project
    QPushButton *m_editButton;                ///< Button for editing a project
    QPushButton *m_deleteButton;              ///< Button for deleting a project
};
