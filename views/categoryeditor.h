/**
 * @file categoryeditor.h
 * @brief Definition of the CategoryEditor class
 * 
 * This file defines the CategoryEditor dialog class which provides a user interface
 * for creating and editing task categories. It allows users to modify the category
 * name and color.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "../models/category.h"

/**
 * @class CategoryEditor
 * @brief Dialog for creating and editing categories
 * 
 * The CategoryEditor class provides a modal dialog that allows users to
 * create new categories or edit existing ones. It includes fields for the
 * category name and color, and validates user input before accepting changes.
 */
class CategoryEditor : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new CategoryEditor dialog.
     * 
     * @param parent Optional parent widget
     */
    explicit CategoryEditor(QWidget *parent = nullptr);

    /**
     * @brief Set the category to edit
     * 
     * Populates the dialog with the properties of the specified category
     * for editing. If creating a new category, this can be called with
     * a default-constructed Category.
     * 
     * @param category The category to edit
     */
    void setCategory(const Category &category);

    /**
     * @brief Get the edited category
     * 
     * Returns a Category object with the current values from the dialog.
     * This should be called after the dialog is accepted to retrieve
     * the user's changes.
     * 
     * @return Category The edited category
     */
    Category category() const;

private slots:
    /**
     * @brief Handle color button click
     * 
     * Opens a color dialog to select a new category color.
     */
    void onColorButtonClicked();
    
    /**
     * @brief Handle dialog acceptance
     * 
     * Called when the user accepts the dialog. Performs validation
     * and updates the category object.
     */
    void onAccepted();

private:
    /**
     * @brief Set up the user interface
     * 
     * Creates and arranges all UI elements.
     */
    void setupUi();
    
    /**
     * @brief Update color button appearance
     * 
     * Updates the color button background to reflect the selected color.
     */
    void updateColorButton();

    Category m_category;        ///< The category being edited
    QColor m_selectedColor;     ///< The currently selected color

    // UI elements
    QLineEdit *m_nameEdit;      ///< Field for category name
    QPushButton *m_colorButton; ///< Button for color selection
};
