/**
 * @file settingsdialog.cpp
 * @brief Implementation of the SettingsDialog class
 * 
 * This file implements the SettingsDialog class which provides a tabbed
 * interface for configuring application settings. It includes tabs for
 * general settings, category management, and notification preferences.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "categoryeditor.h"
#include "projectstab.h"
#include "../services/settingsmanager.h"
#include "categorydelegate.h"

/**
 * @brief Constructor
 * 
 * Creates a new SettingsDialog with the specified category model.
 * Uses the controller singleton instances internally for categories and projects.
 * 
 * @param categoryModel Pointer to the category model
 * @param parent Optional parent widget
 */
SettingsDialog::SettingsDialog(
    CategoryModel *categoryModel,
    QWidget *parent)
    : QDialog(parent), 
      m_categoryModel(categoryModel),
      m_projectModel(ProjectController::instance().model()) // Get model from singleton
{
    setupUi();
    loadSettings();
}

/**
 * @brief Set up the user interface
 * 
 * Creates and configures the main dialog layout and tab widget.
 * Initializes the three main tabs: General, Categories, and Notifications.
 */
void SettingsDialog::setupUi()
{
    setWindowTitle("Settings");
    setMinimumSize(450, 350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);

    // Create tabs
    QWidget *generalTab = new QWidget();
    QWidget *categoriesTab = new QWidget();
    QWidget *projectsTab = new QWidget();
    QWidget *notificationsTab = new QWidget();

    setupGeneralTab(generalTab);
    setupCategoriesTab(categoriesTab);
    setupNotificationsTab(notificationsTab);
    setupProjectsTab(projectsTab);

    m_tabWidget->addTab(generalTab, "General");
    m_tabWidget->addTab(categoriesTab, "Categories");
    m_tabWidget->addTab(notificationsTab, "Notifications");
    m_tabWidget->addTab(projectsTab, "Projects");

    mainLayout->addWidget(m_tabWidget);

    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

/**
 * @brief Set up the General tab
 * 
 * Creates and configures the General tab, which includes settings for:
 * - Window behavior (always on top, opacity)
 * - Startup options (start with Windows, start minimized)
 * - Default category for new tasks
 * 
 * @param tab The widget to use for the General tab
 */
void SettingsDialog::setupGeneralTab(QWidget *tab)
{
    QVBoxLayout *layout = new QVBoxLayout(tab);

    // Window settings
    QGroupBox *windowGroup = new QGroupBox("Window Settings", tab);
    QFormLayout *windowLayout = new QFormLayout(windowGroup);

    m_alwaysOnTopCheck = new QCheckBox("Keep window always on top", windowGroup);
    windowLayout->addRow(m_alwaysOnTopCheck);

    QHBoxLayout *opacityLayout = new QHBoxLayout();
    m_opacitySlider = new QSlider(Qt::Horizontal, windowGroup);
    m_opacitySlider->setRange(50, 100);
    m_opacitySlider->setValue(100);
    m_opacitySlider->setTickPosition(QSlider::TicksBelow);
    m_opacitySlider->setTickInterval(10);

    m_opacityValueLabel = new QLabel("100%", windowGroup);
    m_opacityValueLabel->setMinimumWidth(40);

    opacityLayout->addWidget(m_opacitySlider);
    opacityLayout->addWidget(m_opacityValueLabel);

    windowLayout->addRow("Opacity:", opacityLayout);

    // Startup settings
    QGroupBox *startupGroup = new QGroupBox("Startup Settings", tab);
    QFormLayout *startupLayout = new QFormLayout(startupGroup);

    m_startWithWindowsCheck = new QCheckBox("Start with Windows", startupGroup);
    startupLayout->addRow(m_startWithWindowsCheck);

    // In setupGeneralTab method, add this after m_startWithWindowsCheck
    m_startMinimizedCheck = new QCheckBox("Start minimized to system tray", startupGroup);
    startupLayout->addRow(m_startMinimizedCheck);

    // Default category
    QGroupBox *defaultsGroup = new QGroupBox("Default Settings", tab);
    QFormLayout *defaultsLayout = new QFormLayout(defaultsGroup);

    m_defaultCategoryCombo = new QComboBox(defaultsGroup);
    defaultsLayout->addRow("Default Category:", m_defaultCategoryCombo);

    // Add groups to layout
    layout->addWidget(windowGroup);
    layout->addWidget(startupGroup);
    layout->addWidget(defaultsGroup);
    layout->addStretch();

    // Connect signals
    connect(m_opacitySlider, &QSlider::valueChanged, this, &SettingsDialog::onOpacityChanged);
}

/**
 * @brief Set up the Categories tab
 * 
 * Creates and configures the Categories tab, which includes:
 * - A list view of all categories with their colors
 * - Buttons for adding, editing, and deleting categories
 * 
 * @param tab The widget to use for the Categories tab
 */
void SettingsDialog::setupCategoriesTab(QWidget *tab)
{
    QVBoxLayout *layout = new QVBoxLayout(tab);

    // Category list
    m_categoryListView = new QListView(tab);
    m_categoryListView->setModel(m_categoryModel);
    m_categoryListView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Add the custom delegate
    m_categoryListView->setItemDelegate(new CategoryDelegate(m_categoryListView));

    // Disable editing in the view
    m_categoryListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(m_categoryListView);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *addButton = new QPushButton("Add", tab);
    QPushButton *editButton = new QPushButton("Edit", tab);
    QPushButton *deleteButton = new QPushButton("Delete", tab);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    // Connect signals
    connect(addButton, &QPushButton::clicked, this, &SettingsDialog::onAddCategoryClicked);
    connect(editButton, &QPushButton::clicked, this, &SettingsDialog::onEditCategoryClicked);
    connect(deleteButton, &QPushButton::clicked, this, &SettingsDialog::onDeleteCategoryClicked);
    connect(m_categoryListView, &QListView::doubleClicked, this, &SettingsDialog::onCategoryDoubleClicked);
}

/**
 * @brief Set up the Projects tab
 * 
 * Creates and configures the Projects tab using the ProjectsTab widget.
 * 
 * @param tab The widget to use for the Projects tab
 */
void SettingsDialog::setupProjectsTab(QWidget *tab)
{
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    // Create and add the ProjectsTab widget - now using singleton controller internally
    ProjectsTab *projectsTab = new ProjectsTab(tab);
    layout->addWidget(projectsTab);
}

/**
 * @brief Set up the Notifications tab
 * 
 * Creates and configures the Notifications tab, which includes settings for:
 * - Enabling/disabling notifications
 * - Options for when to show notifications (due soon, overdue)
 * 
 * @param tab The widget to use for the Notifications tab
 */
void SettingsDialog::setupNotificationsTab(QWidget *tab)
{
    QVBoxLayout *layout = new QVBoxLayout(tab);

    // Notification settings
    QGroupBox *notificationGroup = new QGroupBox("Notification Settings", tab);
    QVBoxLayout *notificationLayout = new QVBoxLayout(notificationGroup);

    m_enableNotificationsCheck = new QCheckBox("Enable notifications", notificationGroup);
    m_notifyDueSoonCheck = new QCheckBox("Notify when tasks are due soon (within 1 hour)", notificationGroup);
    m_notifyOverdueCheck = new QCheckBox("Notify when tasks are overdue", notificationGroup);

    notificationLayout->addWidget(m_enableNotificationsCheck);
    notificationLayout->addWidget(m_notifyDueSoonCheck);
    notificationLayout->addWidget(m_notifyOverdueCheck);

    layout->addWidget(notificationGroup);
    layout->addStretch();

    // Connect signals
    connect(m_enableNotificationsCheck, &QCheckBox::toggled, [this](bool checked) {
        m_notifyDueSoonCheck->setEnabled(checked);
        m_notifyOverdueCheck->setEnabled(checked);
    });
}

/**
 * @brief Load settings into the dialog
 * 
 * Retrieves current settings from the SettingsManager and
 * populates the dialog controls with these values.
 */
void SettingsDialog::loadSettings()
{
    // Load window settings
    m_alwaysOnTopCheck->setChecked(SettingsManager::instance().alwaysOnTop());
    m_opacitySlider->setValue(SettingsManager::instance().opacity() * 100);

    // Load startup settings
    m_startWithWindowsCheck->setChecked(SettingsManager::instance().startWithWindows());
    m_startMinimizedCheck->setChecked(SettingsManager::instance().startMinimized());

    // Load notification settings
    bool enableNotifications = SettingsManager::instance().enableNotifications();
    m_enableNotificationsCheck->setChecked(enableNotifications);
    m_notifyDueSoonCheck->setChecked(SettingsManager::instance().notifyDueSoon());
    m_notifyOverdueCheck->setChecked(SettingsManager::instance().notifyOverdue());
    m_notifyDueSoonCheck->setEnabled(enableNotifications);
    m_notifyOverdueCheck->setEnabled(enableNotifications);

    // Populate default category combo
    m_defaultCategoryCombo->clear();

    QList<Category> categories = m_categoryModel->getCategories();
    for (const Category &category : categories) {
        QPixmap pixmap(16, 16);
        pixmap.fill(category.color());

        m_defaultCategoryCombo->addItem(QIcon(pixmap), category.name(), category.id());
    }

    // Set current default category
    QString defaultCategoryId = SettingsManager::instance().defaultCategoryId();
    int index = m_defaultCategoryCombo->findData(defaultCategoryId);
    if (index >= 0) {
        m_defaultCategoryCombo->setCurrentIndex(index);
    }
}

/**
 * @brief Save settings from the dialog
 * 
 * Retrieves values from the dialog controls and saves them
 * to the SettingsManager, which persists them to disk.
 */
void SettingsDialog::saveSettings()
{
    // Save window settings
    SettingsManager::instance().setAlwaysOnTop(m_alwaysOnTopCheck->isChecked());
    SettingsManager::instance().setOpacity(m_opacitySlider->value() / 100.0);

    // Save startup settings
    SettingsManager::instance().setStartWithWindows(m_startWithWindowsCheck->isChecked());
    SettingsManager::instance().setStartMinimized(m_startMinimizedCheck->isChecked());

    // Save notification settings
    SettingsManager::instance().setEnableNotifications(m_enableNotificationsCheck->isChecked());
    SettingsManager::instance().setNotifyDueSoon(m_notifyDueSoonCheck->isChecked());
    SettingsManager::instance().setNotifyOverdue(m_notifyOverdueCheck->isChecked());

    // Save default category
    QString defaultCategoryId = m_defaultCategoryCombo->currentData().toString();
    SettingsManager::instance().setDefaultCategoryId(defaultCategoryId);

    // Save all settings to disk
    SettingsManager::instance().save();
}

/**
 * @brief Handle OK button click
 * 
 * Saves the settings and accepts the dialog when the OK button is clicked.
 */
void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

/**
 * @brief Handle opacity slider value change
 * 
 * Updates the opacity value label when the slider is moved.
 * 
 * @param value The new opacity value (50-100)
 */
void SettingsDialog::onOpacityChanged(int value)
{
    m_opacityValueLabel->setText(QString("%1%").arg(value));
}

/**
 * @brief Handle add category button click
 * 
 * Opens the category editor dialog to create a new category.
 * If the dialog is accepted, adds the new category to the model
 * and updates the UI.
 */
void SettingsDialog::onAddCategoryClicked()
{
    CategoryEditor editor(this);

    if (editor.exec() == QDialog::Accepted) {
        Category category = editor.category();
        // Use the controller to add the category
        CategoryController::instance().addCategory(category.name(), category.color());

        // Update default category combo
        loadSettings();
    }
}

/**
 * @brief Handle edit category button click
 * 
 * Opens the category editor for the currently selected category.
 * Delegates to onCategoryDoubleClicked for the actual editing logic.
 */
void SettingsDialog::onEditCategoryClicked()
{
    QModelIndex currentIndex = m_categoryListView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }

    onCategoryDoubleClicked(currentIndex);
}

/**
 * @brief Handle delete category button click
 * 
 * Deletes the currently selected category after confirmation.
 * Shows a warning if trying to delete a default category.
 */
void SettingsDialog::onDeleteCategoryClicked()
{
    QModelIndex currentIndex = m_categoryListView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }

    QString categoryId = m_categoryModel->data(currentIndex, CategoryModel::IdRole).toString();
    QString categoryName = m_categoryModel->data(currentIndex, CategoryModel::NameRole).toString();

    // Check if this is a default category
    Category category = m_categoryModel->getCategory(categoryId);
    if (category.isDefault()) {
        QMessageBox::warning(this, "Cannot Delete",
                            "Default categories cannot be deleted.",
                            QMessageBox::Ok);
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete the category \"%1\"?\n\nTasks in this category will not be deleted, but will need to be reassigned.").arg(categoryName),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // Use the controller to delete the category
        CategoryController::instance().deleteCategory(categoryId);

        // Update default category combo
        loadSettings();
    }
}

/**
 * @brief Handle category double-click
 * 
 * Opens the category editor dialog when a category is double-clicked.
 * If the dialog is accepted, updates the category through the controller.
 * 
 * @param index The model index of the clicked category
 */
void SettingsDialog::onCategoryDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    QString categoryId = m_categoryModel->data(index, CategoryModel::IdRole).toString();
    Category category = m_categoryModel->getCategory(categoryId);

    CategoryEditor editor(this);
    editor.setCategory(category);

    if (editor.exec() == QDialog::Accepted) {
        Category editedCategory = editor.category();

        // Use the controller instead of directly modifying the model
        CategoryController::instance().updateCategory(categoryId, editedCategory.name(), editedCategory.color());

        // Update default category combo
        loadSettings();
    }
}
