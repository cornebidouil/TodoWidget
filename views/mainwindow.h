/**
 * @file mainwindow.h
 * @brief Definition of the MainWindow class
 * 
 * This file defines the MainWindow class which serves as the main UI container
 * for the TODO Widget application. It integrates all views, models, and controllers
 * and handles user interactions, window state, and system tray functionality.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QMainWindow>
#include <QListView>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include "../models/taskmodel.h"
#include "../models/categorymodel.h"
#include "../controllers/taskcontroller.h"
#include "../controllers/categorycontroller.h"
#include "../controllers/notificationcontroller.h"
#include "taskitemdelegate.h"
#include "tasklistview.h"

/**
 * @class MainWindow
 * @brief Main application window
 * 
 * The MainWindow class is the primary container for the TODO Widget application UI.
 * It manages the task list view, quick add functionality, category filtering,
 * system tray integration, and coordinates between models, views, and controllers.
 * The window supports custom window behavior including frameless dragging and
 * minimize-to-tray functionality.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates and initializes the main window and all its components.
     * 
     * @param parent Optional parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor
     * 
     * Cleans up resources used by the main window.
     */
    ~MainWindow();
    
    /**
     * @brief Show and activate the window
     * 
     * Makes the window visible, brings it to the front, and gives it focus.
     * Used primarily when activating the application from the system tray.
     */
    void showAndActivate();

protected:
    /**
     * @brief Handle window close events
     * 
     * Overrides the default close behavior to support minimizing to tray
     * instead of actually closing the application if that option is enabled.
     * 
     * @param event The close event object
     */
    void closeEvent(QCloseEvent *event) override;
    
    /**
     * @brief Handle mouse press events
     * 
     * Supports dragging the frameless window by tracking the initial click position.
     * 
     * @param event The mouse press event object
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief Handle mouse move events
     * 
     * Moves the frameless window when dragging is active.
     * 
     * @param event The mouse move event object
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    
    /**
     * @brief Handle mouse release events
     * 
     * Terminates dragging operations when the mouse button is released.
     * 
     * @param event The mouse release event object
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief Handle add task button click
     * 
     * Creates a new task from the current content of the quick add text field.
     */
    void onAddTaskClicked();
    
    /**
     * @brief Handle edit task button click
     * 
     * Opens the task editor for the currently selected task.
     */
    void onEditTaskClicked();
    
    /**
     * @brief Handle delete task button click
     * 
     * Deletes the currently selected task after confirmation.
     */
    void onDeleteTaskClicked();
    
    /**
     * @brief Handle clear completed button click
     * 
     * Removes all completed tasks after confirmation.
     */
    void onClearCompletedClicked();
    
    /**
     * @brief Handle task double click
     * 
     * Opens the task editor for the double-clicked task.
     * 
     * @param index Model index of the double-clicked task
     */
    void onTaskDoubleClicked(const QModelIndex &index);
    
    /**
     * @brief Handle category filter change
     * 
     * Filters the task list to show only tasks in the selected category.
     * 
     * @param index Index of the selected category in the combo box
     */
    void onCategoryFilterChanged(int index);
    
    /**
     * @brief Handle settings button click
     * 
     * Opens the settings dialog.
     */
    void onSettingsClicked();
    
    /**
     * @brief Handle system tray icon activation
     * 
     * Responds to clicks on the system tray icon, typically by toggling
     * the visibility of the main window.
     * 
     * @param reason The type of activation that occurred
     */
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    
    /**
     * @brief Handle exit action from tray menu
     * 
     * Completely exits the application when the Exit option is selected
     * from the system tray menu.
     */
    void onExitActionTriggered();
    
    /**
     * @brief Handle context menu request on task list
     * 
     * Shows a context menu with relevant actions for the clicked task.
     * 
     * @param pos The position where the context menu should appear
     */
    void onTaskListContextMenu(const QPoint &pos);
    
    /**
     * @brief Show the add task dialog
     * 
     * Opens the full task editor dialog for creating a new task.
     */
    void onAddTaskWithDialog();
    
    /**
     * @brief Handle tasks being reordered in the view
     * 
     * Updates the model after tasks have been reordered through drag and drop.
     * 
     * @param parent Parent model index
     * @param start Starting row
     * @param end Ending row
     * @param destination Destination parent index
     * @param row Destination row
     */
    void onTasksReordered(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    
    /**
     * @brief Handle task drag and drop
     * 
     * Updates the task order when a task is dragged to a new position.
     * 
     * @param sourceRow Original row of the dragged task
     * @param targetRow Target row where the task was dropped
     */
    void onTaskDropped(int sourceRow, int targetRow);

private:
    // Setup methods
    
    /**
     * @brief Set up the user interface
     * 
     * Creates and arranges all UI elements.
     */
    void setupUi();
    
    /**
     * @brief Set up the data models
     * 
     * Initializes the task and category models.
     */
    void setupModels();
    
    /**
     * @brief Set up the controllers
     * 
     * Initializes the task, category, and notification controllers.
     */
    void setupControllers();
    
    /**
     * @brief Set up signal-slot connections
     * 
     * Connects UI elements to their corresponding slots.
     */
    void setupConnections();
    
    /**
     * @brief Set up the system tray icon and menu
     * 
     * Initializes the system tray functionality.
     */
    void setupSystemTray();
    
    /**
     * @brief Load application settings
     * 
     * Applies saved settings to the window and UI.
     */
    void loadSettings();
    
    /**
     * @brief Save application settings
     * 
     * Persists current window state and settings.
     */
    void saveSettings();
    
    /**
     * @brief Perform delayed initialization
     * 
     * Handles setup tasks that should occur after the window is shown.
     */
    void delayedInitialization();

    // UI update methods
    
    /**
     * @brief Update window flags
     * 
     * Sets the appropriate window flags based on current settings.
     */
    void updateWindowFlags();
    
    /**
     * @brief Update window opacity
     * 
     * Sets the window opacity based on current settings.
     */
    void updateWindowOpacity();
    
    /**
     * @brief Populate the category filter combo box
     * 
     * Updates the combo box with current categories.
     */
    void populateCategoryFilter();

    /**
     * @brief Toggle window visibility
     * 
     * Shows or hides the window based on its current state.
     */
    void toggleVisibility();

    // Models
    TaskModel *m_taskModel;         ///< Model for tasks
    CategoryModel *m_categoryModel; ///< Model for categories

    // Controllers
    TaskController *m_taskController;               ///< Controller for task operations
    CategoryController *m_categoryController;       ///< Controller for category operations
    NotificationController *m_notificationController; ///< Controller for task notifications

    // UI Elements
    TaskListView *m_taskListView;        ///< Custom view for displaying tasks
    QLineEdit *m_quickAddEdit;           ///< Text field for quickly adding new tasks
    QComboBox *m_categoryFilterCombo;    ///< Dropdown for filtering tasks by category
    QPushButton *m_addTaskButton;        ///< Button for adding new tasks
    QPushButton *m_settingsButton;       ///< Button for opening settings dialog

    // System Tray
    QSystemTrayIcon *m_trayIcon;    ///< System tray icon
    QMenu *m_trayMenu;              ///< Menu displayed when right-clicking the tray icon

    // Dragging
    bool m_isDragging;              ///< Flag indicating if the window is being dragged
    QPoint m_dragPosition;          ///< Starting position for window dragging
};
