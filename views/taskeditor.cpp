/**
 * @file taskeditor.cpp
 * @brief Implementation of the TaskEditor class
 * 
 * This file implements the TaskEditor class which provides a dialog
 * for creating and editing task items. It includes form fields for all
 * task properties including title, category, due date, priority, and description.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "taskeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QUuid>

/**
 * @brief Constructor
 * 
 * Creates a new TaskEditor dialog with the specified category model.
 * The category model is used to populate the category dropdown.
 * 
 * @param categoryModel Pointer to the CategoryModel to use for category options
 * @param parent Optional parent widget
 */
TaskEditor::TaskEditor(CategoryModel *categoryModel, QWidget *parent)
    : QDialog(parent), m_categoryModel(categoryModel)
{
    setupUi();
    populateCategories();
}

/**
 * @brief Set up the user interface
 * 
 * Creates and configures all UI elements including form fields,
 * buttons, and layout. Also connects signals and slots for interactive
 * elements like the due date checkbox and priority slider.
 */
void TaskEditor::setupUi()
{
    setWindowTitle("Edit Task");
    setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create form layout
    QFormLayout *formLayout = new QFormLayout();

    // Title field
    m_titleEdit = new QLineEdit(this);
    formLayout->addRow("Title:", m_titleEdit);

    // Category dropdown
    m_categoryCombo = new QComboBox(this);
    formLayout->addRow("Category:", m_categoryCombo);

    // Due date with checkbox
    QHBoxLayout *dueDateLayout = new QHBoxLayout();
    m_hasDueDateCheck = new QCheckBox("Has due date", this);
    m_dueDateEdit = new QDateTimeEdit(this);
    m_dueDateEdit->setCalendarPopup(true);
    m_dueDateEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    m_dueDateEdit->setEnabled(false);

    dueDateLayout->addWidget(m_hasDueDateCheck);
    dueDateLayout->addWidget(m_dueDateEdit, 1);
    formLayout->addRow("Due Date:", dueDateLayout);

    // Priority slider
    QHBoxLayout *priorityLayout = new QHBoxLayout();
    m_prioritySlider = new QSlider(Qt::Horizontal, this);
    m_prioritySlider->setRange(1, 5);
    m_prioritySlider->setValue(3);
    m_prioritySlider->setTickPosition(QSlider::TicksBelow);
    m_prioritySlider->setTickInterval(1);

    m_priorityLabel = new QLabel("Normal", this);
    m_priorityLabel->setMinimumWidth(80);

    priorityLayout->addWidget(m_prioritySlider);
    priorityLayout->addWidget(m_priorityLabel);
    formLayout->addRow("Priority:", priorityLayout);

    // Description field
    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setPlaceholderText("Enter task description here...");
    formLayout->addRow("Description:", m_descriptionEdit);

    mainLayout->addLayout(formLayout);

    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &TaskEditor::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(m_hasDueDateCheck, &QCheckBox::toggled, m_dueDateEdit, &QDateTimeEdit::setEnabled);
    connect(m_prioritySlider, &QSlider::valueChanged, this, &TaskEditor::updatePriorityLabel);

    // Initialize priority label
    updatePriorityLabel(m_prioritySlider->value());
}

/**
 * @brief Populate the category dropdown
 * 
 * Fills the category dropdown with available categories from the model.
 * Each category is displayed with its name and color indicator.
 */
void TaskEditor::populateCategories()
{
    m_categoryCombo->clear();

    QList<Category> categories = m_categoryModel->getCategories();
    for (const Category &category : categories) {
        QPixmap pixmap(16, 16);
        pixmap.fill(category.color());

        m_categoryCombo->addItem(QIcon(pixmap), category.name(), category.id());
    }
}

/**
 * @brief Set the task to edit
 * 
 * Populates the form fields with data from an existing task.
 * This is used when editing an existing task rather than creating a new one.
 * 
 * @param task The Task object to edit
 */
void TaskEditor::setTask(const Task &task)
{
    m_task = task;

    // Populate fields with task data
    m_titleEdit->setText(task.title());
    m_descriptionEdit->setText(task.description());

    // Set category
    int categoryIndex = m_categoryCombo->findData(task.categoryId());
    if (categoryIndex >= 0) {
        m_categoryCombo->setCurrentIndex(categoryIndex);
    }

    // Set due date
    if (task.dueDate().isValid()) {
        m_hasDueDateCheck->setChecked(true);
        m_dueDateEdit->setDateTime(task.dueDate());
        m_dueDateEdit->setEnabled(true);
    } else {
        m_hasDueDateCheck->setChecked(false);
        m_dueDateEdit->setEnabled(false);
    }

    // Set priority
    m_prioritySlider->setValue(task.priority());
}

/**
 * @brief Get the edited task
 * 
 * Creates a Task object from the current form data.
 * If this is a new task, generates a new UUID for it.
 * 
 * @return Task The task with updated properties
 */
Task TaskEditor::task() const
{
    Task task = m_task;

    // If this is a new task, generate an ID
    if (task.id().isEmpty()) {
        task.setId(QUuid::createUuid().toString().remove('{').remove('}'));
        task.setCreatedDate(QDateTime::currentDateTime());
    }

    // Update task with form data
    task.setTitle(m_titleEdit->text());
    task.setDescription(m_descriptionEdit->toPlainText());
    task.setCategoryId(m_categoryCombo->currentData().toString());

    if (m_hasDueDateCheck->isChecked()) {
        task.setDueDate(m_dueDateEdit->dateTime());
    } else {
        task.setDueDate(QDateTime());
    }

    task.setPriority(m_prioritySlider->value());

    return task;
}

/**
 * @brief Handle OK button click
 * 
 * Validates the form data and accepts the dialog if valid.
 * Currently only checks that the title is not empty.
 */
void TaskEditor::onAccepted()
{
    // Validate input
    if (m_titleEdit->text().trimmed().isEmpty()) {
        m_titleEdit->setFocus();
        return;
    }

    accept();
}

/**
 * @brief Update the priority label text
 * 
 * Updates the text label next to the priority slider to show
 * a text representation of the current priority level.
 * 
 * @param value The current priority value (1-5)
 */
void TaskEditor::updatePriorityLabel(int value)
{
    QString priorityText;

    switch (value) {
        case 1: priorityText = "Very Low"; break;
        case 2: priorityText = "Low"; break;
        case 3: priorityText = "Normal"; break;
        case 4: priorityText = "High"; break;
        case 5: priorityText = "Very High"; break;
        default: priorityText = "Normal";
    }

    m_priorityLabel->setText(priorityText);
}
