/**
 * @file projecteditor.h
 * @brief Definition of the ProjectEditor class
 * 
 * This file defines the ProjectEditor class which provides a dialog
 * for creating and editing projects.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QColorDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "../models/project.h"

/**
 * @class ProjectEditor
 * @brief Dialog for creating and editing projects
 * 
 * The ProjectEditor class provides a form for entering project details,
 * including name, color, and description.
 */
class ProjectEditor : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor for creating a new project
     * 
     * Creates a dialog for adding a new project.
     * 
     * @param parent Parent widget
     */
    explicit ProjectEditor(QWidget* parent = nullptr);
    
    /**
     * @brief Constructor for editing an existing project
     * 
     * Creates a dialog pre-populated with the details of an existing project.
     * 
     * @param project The project to edit
     * @param parent Parent widget
     */
    ProjectEditor(const Project& project, QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~ProjectEditor();
    
    /**
     * @brief Get the project from the dialog
     * 
     * Constructs a Project object from the current form values.
     * 
     * @return Project The project with values from the form
     */
    Project project() const;
    
    /**
     * @brief Set the project to edit
     * 
     * Populates the form with values from an existing project.
     * 
     * @param project The project to edit
     */
    void setProject(const Project& project);
    
private slots:
    /**
     * @brief Handle color button click
     * 
     * Opens a color dialog to select a color.
     */
    void onColorButtonClicked();
    
    /**
     * @brief Validate the form inputs
     * 
     * Validates the form inputs before accepting the dialog.
     */
    void validate();
    
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
    
    /**
     * @brief Update the color button
     * 
     * Updates the color button's background color.
     */
    void updateColorButton();
    
    // UI elements
    QLineEdit* m_nameEdit;          ///< Line edit for project name
    QTextEdit* m_descriptionEdit;   ///< Text edit for project description
    QPushButton* m_colorButton;     ///< Button for selecting color
    QCheckBox* m_activeCheckBox;    ///< Checkbox for active state
    QDialogButtonBox* m_buttonBox;  ///< Dialog button box
    
    // State
    QColor m_color;                 ///< Selected color
    bool m_isEditMode;              ///< Flag indicating if editing an existing project
    QString m_projectId;            ///< ID of the project being edited
};
