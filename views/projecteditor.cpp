/**
 * @file projecteditor.cpp
 * @brief Implementation of the ProjectEditor class
 * 
 * The ProjectEditor class provides a dialog for creating and editing Project objects.
 * It allows users to set the project name, color, description, and active status.
 * The class can be used in two modes: add mode for creating new projects and
 * edit mode for modifying existing ones.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "projecteditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QUuid>

/**
 * @brief Constructor for creating a new project
 * 
 * Initializes the dialog in "add" mode with default values.
 * 
 * @param parent The parent widget
 */
ProjectEditor::ProjectEditor(QWidget* parent)
    : QDialog(parent)
    , m_color(Qt::blue)
    , m_isEditMode(false)
    , m_projectId(QUuid::createUuid().toString().remove('{').remove('}'))
{
    setWindowTitle("Add Project");
    setupUi();
    setupConnections();
}

/**
 * @brief Constructor for editing an existing project
 * 
 * Initializes the dialog in "edit" mode with values from the provided project.
 * 
 * @param project The Project object to edit
 * @param parent The parent widget
 */
ProjectEditor::ProjectEditor(const Project& project, QWidget* parent)
    : QDialog(parent)
    , m_color(project.color())
    , m_isEditMode(true)
    , m_projectId(project.id())
{
    setWindowTitle("Edit Project");
    setupUi();
    setupConnections();
    setProject(project);
}

/**
 * @brief Destructor for ProjectEditor
 */
ProjectEditor::~ProjectEditor()
{
}

/**
 * @brief Set up the user interface
 * 
 * Creates and arranges all UI elements for the dialog.
 */
void ProjectEditor::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();
    
    // Project name
    m_nameEdit = new QLineEdit(this);
    formLayout->addRow("Name:", m_nameEdit);
    
    // Project color
    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(30, 30);
    updateColorButton();
    formLayout->addRow("Color:", m_colorButton);
    
    // Project description
    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setMaximumHeight(100);
    formLayout->addRow("Description:", m_descriptionEdit);
    
    // Active status
    m_activeCheckBox = new QCheckBox("Active", this);
    m_activeCheckBox->setChecked(true);
    formLayout->addRow("", m_activeCheckBox);
    
    mainLayout->addLayout(formLayout);
    
    // Button box
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(m_buttonBox);
    
    setMinimumWidth(350);
}

/**
 * @brief Set up signal/slot connections
 * 
 * Connects UI elements to their respective slots.
 */
void ProjectEditor::setupConnections()
{
    connect(m_colorButton, &QPushButton::clicked, this, &ProjectEditor::onColorButtonClicked);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &ProjectEditor::validate);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

/**
 * @brief Handle color button click
 * 
 * Opens a color dialog and updates the selected color if the user chooses one.
 */
void ProjectEditor::onColorButtonClicked()
{
    QColor newColor = QColorDialog::getColor(m_color, this, "Select Project Color");
    if (newColor.isValid()) {
        m_color = newColor;
        updateColorButton();
    }
}

/**
 * @brief Update the color button appearance
 * 
 * Updates the color button to display the currently selected color.
 */
void ProjectEditor::updateColorButton()
{
    QPixmap pixmap(m_colorButton->size());
    pixmap.fill(m_color);
    
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 1);
    
    m_colorButton->setIcon(QIcon(pixmap));
    m_colorButton->setIconSize(pixmap.size());
}

/**
 * @brief Validate user input
 * 
 * Checks if the input is valid and accepts the dialog if it is.
 * Currently only checks that the project name is not empty.
 */
void ProjectEditor::validate()
{
    QString name = m_nameEdit->text().trimmed();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Project name cannot be empty.");
        return;
    }
    
    accept();
}

/**
 * @brief Get the project with user-entered values
 * 
 * Creates and returns a Project object with the values entered by the user.
 * 
 * @return A Project object with the user-entered values
 */
Project ProjectEditor::project() const
{
    QString name = m_nameEdit->text().trimmed();
    QString description = m_descriptionEdit->toPlainText().trimmed();
    bool isActive = m_activeCheckBox->isChecked();
    
    Project project(name, m_color, description);
    project.setId(m_projectId);
    project.setActive(isActive);
    
    return project;
}

/**
 * @brief Set the dialog values from a Project object
 * 
 * Populates the UI elements with values from the provided Project object.
 * 
 * @param project The Project object to get values from
 */
void ProjectEditor::setProject(const Project& project)
{
    m_projectId = project.id();
    m_nameEdit->setText(project.name());
    m_descriptionEdit->setText(project.description());
    m_color = project.color();
    m_activeCheckBox->setChecked(project.isActive());
    
    updateColorButton();
}
