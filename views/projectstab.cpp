/**
 * @file projectstab.cpp
 * @brief Implementation of the ProjectsTab class
 * 
 * The ProjectsTab class provides a widget for managing projects in the application.
 * It displays a list of projects and allows the user to add, edit, and delete them.
 * This tab is typically embedded in the main application window.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#include "projectstab.h"
#include "projecteditor.h"
#include "projectdelegate.h"
#include <QMessageBox>

/**
 * @brief Constructor for ProjectsTab
 * 
 * Initializes the widget and sets up the UI elements and connections.
 * 
 * @param parent The parent widget
 */
ProjectsTab::ProjectsTab(QWidget *parent)
    : QWidget(parent), m_projectModel(ProjectController::instance().model())
{
    // Ensure we're using the singleton controller's model
    setupUi();
    setupConnections();
    
    // Make sure we have the latest data
    refreshProjects();
}

/**
 * @brief Set up the user interface
 * 
 * Creates and arranges all UI elements for the widget.
 */
void ProjectsTab::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Project list
    m_projectListView = new QListView(this);
    m_projectListView->setModel(m_projectModel);
    m_projectListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_projectListView->setItemDelegate(new ProjectDelegate(m_projectListView));
    m_projectListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    layout->addWidget(m_projectListView);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton("Add", this);
    m_editButton = new QPushButton("Edit", this);
    m_deleteButton = new QPushButton("Delete", this);
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
}

/**
 * @brief Set up signal/slot connections
 * 
 * Connects UI elements to their respective slots.
 */
void ProjectsTab::setupConnections()
{
    connect(m_addButton, &QPushButton::clicked, this, &ProjectsTab::onAddProjectClicked);
    connect(m_editButton, &QPushButton::clicked, this, &ProjectsTab::onEditProjectClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &ProjectsTab::onDeleteProjectClicked);
    connect(m_projectListView, &QListView::doubleClicked, this, &ProjectsTab::onProjectDoubleClicked);
}

/**
 * @brief Refresh the projects list
 * 
 * Reloads projects from the database and updates the view.
 */
void ProjectsTab::refreshProjects()
{
    // First, reload projects from the database via the controller
    ProjectController::instance().loadProjects();
    
    // Then refresh the model's view
    m_projectModel->refresh();
}

/**
 * @brief Handle Add Project button click
 * 
 * Opens a ProjectEditor dialog in "add" mode and adds the new project
 * if the user accepts the dialog.
 */
void ProjectsTab::onAddProjectClicked()
{
    ProjectEditor editor(this);
    
    if (editor.exec() == QDialog::Accepted) {
        Project project = editor.project();
        ProjectController::instance().addProject(project);
        
        refreshProjects();
    }
}

/**
 * @brief Handle Edit button click
 * 
 * Opens a ProjectEditor dialog for the currently selected project.
 * Does nothing if no project is selected.
 */
void ProjectsTab::onEditProjectClicked()
{
    QModelIndex currentIndex = m_projectListView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }
    
    onProjectDoubleClicked(currentIndex);
}

/**
 * @brief Handle Delete button click
 * 
 * Confirms with the user and then deletes the currently selected project.
 * Does nothing if no project is selected.
 * 
 * Cannot delete projects that have associated time entries.
 */
void ProjectsTab::onDeleteProjectClicked()
{
    QModelIndex currentIndex = m_projectListView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }
    
    QString projectId = m_projectModel->data(currentIndex, ProjectModel::IdRole).toString();
    QString projectName = m_projectModel->data(currentIndex, ProjectModel::NameRole).toString();
    
    // Check for associated time entries before deleting
    QList<TimeEntry> associatedEntries = ProjectController::instance().getTimeEntriesForProject(projectId);
    
    if (!associatedEntries.isEmpty()) {
        QMessageBox::warning(this, "Cannot Delete",
                           QString("Project '%1' has %2 associated time entries and cannot be deleted. "
                                  "Make the project inactive instead if you no longer wish to use it.")
                                  .arg(projectName)
                                  .arg(associatedEntries.size()),
                           QMessageBox::Ok);
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete the project '%1'?").arg(projectName),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        ProjectController::instance().deleteProject(projectId);
        
        refreshProjects();
    }
}

/**
 * @brief Handle project double-click
 * 
 * Opens a ProjectEditor dialog in "edit" mode for the clicked project.
 * 
 * @param index The model index of the clicked project
 */
void ProjectsTab::onProjectDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    
    QString projectId = m_projectModel->data(index, ProjectModel::IdRole).toString();
    Project project = m_projectModel->getProject(projectId);
    
    ProjectEditor editor(project, this);
    
    if (editor.exec() == QDialog::Accepted) {
        Project updatedProject = editor.project();
        ProjectController::instance().updateProject(updatedProject);
        
        refreshProjects();
    }
}
