# MainWindow.cpp Documentation

## File Overview
```cpp
/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class
 * 
 * This file implements the MainWindow class which serves as the main UI for the TODO Widget
 * application. It manages the task list view, category filtering, system tray integration,
 * and coordinates between controllers and models.
 * 
 * @author Your Name
 * @date Last updated: April 28, 2025
 */
```

## Constructor and Destructor
```cpp
/**
 * @brief Constructor
 * 
 * Creates a new MainWindow instance and sets up the UI, models, controllers,
 * connections, and system tray integration. Also loads settings and initializes
 * the notification controller.
 * 
 * @param parent Optional parent widget
 */
MainWindow::MainWindow(QWidget *parent)

/**
 * @brief Destructor
 * 
 * Saves window settings before destruction.
 */
MainWindow::~MainWindow()
```

## Window Management Methods
```cpp
/**
 * @brief Show the window and bring it to front
 * 
 * If the window is minimized, restores it to normal state,
 * makes it visible, and brings it to the front.
 */
void MainWindow::showAndActivate()

/**
 * @brief Handle close event
 * 
 * Overrides the default close event behavior to minimize to system tray
 * instead of closing the application when the close button is clicked.
 * 
 * @param event The close event
 */
void MainWindow::closeEvent(QCloseEvent *event)

/**
 * @brief Handle mouse press event
 * 
 * Used for implementing window dragging since the window is frameless.
 * Starts dragging when the left mouse button is pressed.
 * 
 * @param event The mouse press event
 */
void MainWindow::mousePressEvent(QMouseEvent *event)

/**
 * @brief Handle mouse move event
 * 
 * Used for implementing window dragging since the window is frameless.
 * Moves the window when the left mouse button is pressed and dragged.
 * 
 * @param event The mouse move event
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)

/**
 * @brief Handle mouse release event
 * 
 * Used for implementing window dragging since the window is frameless.
 * Stops dragging when the left mouse button is released.
 * 
 * @param event The mouse release event
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *event)

/**
 * @brief Toggle window visibility
 * 
 * Shows or hides the window based on its current visibility state.
 * If visible, hides it; if hidden, shows it and activates it.
 */
void MainWindow::toggleVisibility()
```

## Setup Methods
```cpp
/**
 * @brief Delayed initialization
 * 
 * Performs initialization operations that should be performed
 * after a delay to improve application startup time.
 */
void MainWindow::delayedInitialization()

/**
 * @brief Set up the user interface
 * 
 * Creates and configures all UI elements including window properties,
 * layouts, controls, and connections.
 */
void MainWindow::setupUi()

/**
 * @brief Set up data models
 * 
 * Creates and initializes the task and category models,
 * and configures the task list view with the appropriate model and delegate.
 */
void MainWindow::setupModels()

/**
 * @brief Set up controllers
 * 
 * Creates and initializes the task, category, and notification controllers.
 */
void MainWindow::setupControllers()

/**
 * @brief Set up signal-slot connections
 * 
 * Establishes connections between UI elements, models, and controllers
 * to handle user interactions and updates.
 */
void MainWindow::setupConnections()

/**
 * @brief Set up system tray integration
 * 
 * Creates and configures the system tray icon and its context menu.
 */
void MainWindow::setupSystemTray()
```

## Settings Management
```cpp
/**
 * @brief Load application settings
 * 
 * Retrieves window position, size, and other settings from the
 * SettingsManager and applies them to the UI.
 */
void MainWindow::loadSettings()

/**
 * @brief Save application settings
 * 
 * Saves the current window position, size, and other settings
 * to the SettingsManager.
 */
void MainWindow::saveSettings()

/**
 * @brief Update window flags
 * 
 * Updates the window flags based on current settings,
 * such as whether the window should always be on top.
 */
void MainWindow::updateWindowFlags()

/**
 * @brief Update window opacity
 * 
 * Sets the window opacity based on the current settings.
 */
void MainWindow::updateWindowOpacity()
```

## Category Management
```cpp
/**
 * @brief Populate category filter dropdown
 * 
 * Fills the category filter dropdown with all available categories,
 * preserving the current selection if possible.
 */
void MainWindow::populateCategoryFilter()

/**
 * @brief Handle category filter change
 * 
 * Updates the task list to show only tasks in the selected category.
 * If "All Categories" is selected, shows all tasks.
 * 
 * @param index Index of the selected category in the dropdown
 */
void MainWindow::onCategoryFilterChanged(int index)
```

## Task Management
```cpp
/**
 * @brief Handle add task button click
 * 
 * Adds a new task with the text from the quick add edit field.
 */
void MainWindow::onAddTaskClicked()

/**
 * @brief Handle edit task action
 * 
 * Opens the task editor for the currently selected task.
 */
void MainWindow::onEditTaskClicked()

/**
 * @brief Handle delete task action
 * 
 * Deletes the currently selected task after confirmation.
 */
void MainWindow::onDeleteTaskClicked()

/**
 * @brief Handle clear completed tasks action
 * 
 * Deletes all completed tasks after confirmation.
 */
void MainWindow::onClearCompletedClicked()

/**
 * @brief Handle task double-click
 * 
 * Opens the task editor when a task is double-clicked.
 * 
 * @param index Model index of the clicked task
 */
void MainWindow::onTaskDoubleClicked(const QModelIndex &index)

/**
 * @brief Handle add task with dialog
 * 
 * Opens the task editor dialog to create a new task with more details.
 */
void MainWindow::onAddTaskWithDialog()
```

## Context Menu
```cpp
/**
 * @brief Handle task list context menu request
 * 
 * Shows a context menu with task management options when right-clicking
 * on the task list.
 * 
 * @param pos Position where the context menu was requested
 */
void MainWindow::onTaskListContextMenu(const QPoint &pos)
```

## UI Event Handlers
```cpp
/**
 * @brief Handle settings button click
 * 
 * Opens the settings dialog and applies any changes.
 */
void MainWindow::onSettingsClicked()

/**
 * @brief Handle tray icon activation
 * 
 * Toggles window visibility when the tray icon is clicked.
 * 
 * @param reason Reason for the activation
 */
void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)

/**
 * @brief Handle exit action
 * 
 * Quits the application when the exit action is triggered.
 */
void MainWindow::onExitActionTriggered()
```

## Drag and Drop
```cpp
/**
 * @brief Handle task reordering
 * 
 * Updates the task order when tasks are reordered via the model.
 * 
 * @param parent Parent model index
 * @param start Start row
 * @param end End row
 * @param destination Destination model index
 * @param row Destination row
 */
void MainWindow::onTasksReordered(const QModelIndex &parent, int start, int end, 
    const QModelIndex &destination, int row)

/**
 * @brief Handle task drop
 * 
 * Updates the task order when a task is dragged and dropped.
 * 
 * @param sourceRow Source row
 * @param targetRow Target row
 */
void MainWindow::onTaskDropped(int sourceRow, int targetRow)
```
