/**
 * @file categoryeditor.cpp
 * @brief Implementation of the CategoryEditor class
 * 
 * This file implements the CategoryEditor class which provides a dialog
 * for creating and editing task categories. It includes controls for
 * setting a category's name and color.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "categoryeditor.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QUuid>

/**
 * @brief Constructor
 * 
 * Creates a new CategoryEditor dialog with default values.
 * Sets up the UI and initializes the default color to blue.
 * 
 * @param parent Optional parent widget
 */
CategoryEditor::CategoryEditor(QWidget *parent)
    : QDialog(parent)
{
    setupUi();

    // Default color
    m_selectedColor = QColor(0, 120, 215); // Default blue
    updateColorButton();
}

/**
 * @brief Set up the user interface
 * 
 * Creates and configures all UI elements including the name field,
 * color selector button, and dialog buttons.
 */
void CategoryEditor::setupUi()
{
    setWindowTitle("Edit Category");
    setMinimumWidth(300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create form layout
    QFormLayout *formLayout = new QFormLayout();

    // Name field
    m_nameEdit = new QLineEdit(this);
    formLayout->addRow("Name:", m_nameEdit);

    // Color button
    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(80, 30);
    formLayout->addRow("Color:", m_colorButton);

    mainLayout->addLayout(formLayout);

    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CategoryEditor::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(m_colorButton, &QPushButton::clicked, this, &CategoryEditor::onColorButtonClicked);
}

/**
 * @brief Set the category to edit
 * 
 * Populates the form fields with data from an existing category.
 * This is used when editing an existing category rather than creating a new one.
 * 
 * @param category The Category object to edit
 */
void CategoryEditor::setCategory(const Category &category)
{
    m_category = category;

    // Populate fields with category data
    m_nameEdit->setText(category.name());
    m_selectedColor = category.color();
    updateColorButton();
}

/**
 * @brief Get the edited category
 * 
 * Creates a Category object from the current form data.
 * If this is a new category, generates a new UUID for it.
 * 
 * @return Category The category with updated properties
 */
Category CategoryEditor::category() const
{
    Category category = m_category;

    // If this is a new category, generate an ID
    if (category.id().isEmpty()) {
        category.setId(QUuid::createUuid().toString().remove('{').remove('}'));
    }

    // Update category with form data
    category.setName(m_nameEdit->text());
    category.setColor(m_selectedColor);

    return category;
}

/**
 * @brief Handle color button click
 * 
 * Opens a color selection dialog when the color button is clicked.
 * Updates the button appearance and selected color when a color is chosen.
 */
void CategoryEditor::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_selectedColor, this, "Select Category Color");

    if (color.isValid()) {
        m_selectedColor = color;
        updateColorButton();
    }
}

/**
 * @brief Handle OK button click
 * 
 * Validates the form data and accepts the dialog if valid.
 * Currently only checks that the name is not empty.
 */
void CategoryEditor::onAccepted()
{
    // Validate input
    if (m_nameEdit->text().trimmed().isEmpty()) {
        m_nameEdit->setFocus();
        return;
    }

    accept();
}

/**
 * @brief Update the color button appearance
 * 
 * Updates the color button's background color to reflect
 * the currently selected color.
 */
void CategoryEditor::updateColorButton()
{
    QString styleSheet = QString("background-color: %1; border: 1px solid #c0c0c0;").arg(m_selectedColor.name());
    m_colorButton->setStyleSheet(styleSheet);
}
