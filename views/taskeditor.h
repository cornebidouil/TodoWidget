/**
 * @file taskeditor.h
 * @brief Definition of the TaskEditor class
 * 
 * This file defines the TaskEditor dialog class which provides a user interface
 * for creating and editing tasks. It allows users to modify all task properties
 * including title, description, category, due date, and priority.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include "../models/task.h"
#include "../models/categorymodel.h"

/**
 * @class TaskEditor
 * @brief Dialog for creating and editing tasks
 * 
 * The TaskEditor class provides a modal dialog that allows users to
 * create new tasks or edit existing ones. It includes fields for all
 * task properties and validates user input before accepting changes.
 */
class TaskEditor : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new TaskEditor dialog with the specified category model
     * for category selection.
     * 
     * @param categoryModel Pointer to the category model
     * @param parent Optional parent widget
     */
    explicit TaskEditor(CategoryModel *categoryModel, QWidget *parent = nullptr);

    /**
     * @brief Set the task to edit
     * 
     * Populates the dialog with the properties of the specified task
     * for editing. If creating a new task, this can be called with
     * a default-constructed Task.
     * 
     * @param task The task to edit
     */
    void setTask(const Task &task);

    /**
     * @brief Get the edited task
     * 
     * Returns a Task object with the current values from the dialog.
     * This should be called after the dialog is accepted to retrieve
     * the user's changes.
     * 
     * @return Task The edited task
     */
    Task task() const;

private slots:
    /**
     * @brief Handle dialog acceptance
     * 
     * Called when the user accepts the dialog. Performs validation
     * and updates the task object.
     */
    void onAccepted();
    
    /**
     * @brief Update priority label
     * 
     * Updates the priority label text based on the current slider value.
     * 
     * @param value The current priority value (1-5)
     */
    void updatePriorityLabel(int value);

private:
    /**
     * @brief Set up the user interface
     * 
     * Creates and arranges all UI elements.
     */
    void setupUi();
    
    /**
     * @brief Populate the category combo box
     * 
     * Fills the category combo box with available categories.
     */
    void populateCategories();

    Task m_task;                     ///< The task being edited
    CategoryModel *m_categoryModel;  ///< Pointer to the category model

    // UI elements
    QLineEdit *m_titleEdit;          ///< Field for task title
    QTextEdit *m_descriptionEdit;    ///< Field for task description
    QComboBox *m_categoryCombo;      ///< Dropdown for task category
    QDateTimeEdit *m_dueDateEdit;    ///< Date/time picker for due date
    QCheckBox *m_hasDueDateCheck;    ///< Checkbox to enable/disable due date
    QSlider *m_prioritySlider;       ///< Slider for priority (1-5)
    QLabel *m_priorityLabel;         ///< Label showing priority description
};
