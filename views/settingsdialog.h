/**
 * @file settingsdialog.h
 * @brief Definition of the SettingsDialog class
 * 
 * This file defines the SettingsDialog class which provides a user interface
 * for configuring application settings such as appearance, startup behavior,
 * categories, and notifications.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QLabel>
#include <QDialog>
#include <QCheckBox>
#include <QGroupBox>
#include <QSlider>
#include <QComboBox>
#include <QTabWidget>
#include <QListView>
#include "../models/categorymodel.h"
#include "../controllers/categorycontroller.h"
#include "../models/projectmodel.h"
#include "../controllers/projectcontroller.h"

/**
 * @class SettingsDialog
 * @brief Dialog for configuring application settings
 * 
 * The SettingsDialog class provides a tabbed dialog for configuring various
 * aspects of the application, including:
 * - General settings (appearance, startup behavior)
 * - Categories management
 * - Notification preferences
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new SettingsDialog with the specified category model.
     * Uses the controller singleton instances internally.
     * 
     * @param categoryModel Pointer to the category model
     * @param parent Optional parent widget
     */
    explicit SettingsDialog(
        CategoryModel *categoryModel, 
        QWidget *parent = nullptr);

private slots:
    /**
     * @brief Handle dialog acceptance
     * 
     * Called when the user accepts the dialog. Saves all settings.
     */
    void onAccepted();
    
    /**
     * @brief Handle opacity slider change
     * 
     * Updates the opacity value label when the slider changes.
     * 
     * @param value The new opacity value (0-100)
     */
    void onOpacityChanged(int value);
    
    /**
     * @brief Handle add category button click
     * 
     * Opens the category editor dialog to add a new category.
     */
    void onAddCategoryClicked();
    
    /**
     * @brief Handle edit category button click
     * 
     * Opens the category editor dialog to edit the selected category.
     */
    void onEditCategoryClicked();
    
    /**
     * @brief Handle delete category button click
     * 
     * Deletes the selected category after confirmation.
     */
    void onDeleteCategoryClicked();
    
    /**
     * @brief Handle category double click
     * 
     * Opens the category editor for the double-clicked category.
     * 
     * @param index Model index of the double-clicked category
     */
    void onCategoryDoubleClicked(const QModelIndex &index);

private:
    /**
     * @brief Set up the user interface
     * 
     * Creates and arranges all UI elements.
     */
    void setupUi();
    
    /**
     * @brief Set up the general settings tab
     * 
     * Creates and arranges UI elements for the general settings tab.
     * 
     * @param tab Widget container for the tab
     */
    void setupGeneralTab(QWidget *tab);
    
    /**
     * @brief Set up the categories tab
     * 
     * Creates and arranges UI elements for the categories tab.
     * 
     * @param tab Widget container for the tab
     */
    void setupCategoriesTab(QWidget *tab);
    
    /**
     * @brief Set up the projects tab
     * 
     * Creates and arranges UI elements for the projects tab.
     * 
     * @param tab Widget container for the tab
     */
    void setupProjectsTab(QWidget *tab);
    
    /**
     * @brief Set up the notifications tab
     * 
     * Creates and arranges UI elements for the notifications tab.
     * 
     * @param tab Widget container for the tab
     */
    void setupNotificationsTab(QWidget *tab);

    /**
     * @brief Load settings from storage
     * 
     * Loads current settings and updates UI elements.
     */
    void loadSettings();
    
    /**
     * @brief Save settings to storage
     * 
     * Saves settings from UI elements to persistent storage.
     */
    void saveSettings();

    CategoryModel *m_categoryModel;           ///< Pointer to the category model
    ProjectModel *m_projectModel;             ///< Pointer to the project model

    // UI elements
    QTabWidget *m_tabWidget;              ///< Tab container for settings sections
    QCheckBox *m_alwaysOnTopCheck;        ///< Checkbox for always-on-top setting
    QSlider *m_opacitySlider;             ///< Slider for window opacity
    QLabel *m_opacityValueLabel;          ///< Label showing opacity percentage
    QCheckBox *m_startWithWindowsCheck;   ///< Checkbox for auto-start with Windows
    QComboBox *m_defaultCategoryCombo;    ///< Dropdown for default category

    QListView *m_categoryListView;        ///< List view for categories

    QCheckBox *m_enableNotificationsCheck; ///< Checkbox for enabling notifications
    QCheckBox *m_notifyDueSoonCheck;       ///< Checkbox for due soon notifications
    QCheckBox *m_notifyOverdueCheck;       ///< Checkbox for overdue notifications

    QCheckBox *m_startMinimizedCheck;      ///< Checkbox for start minimized setting
};
