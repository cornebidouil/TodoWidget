/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class
 * 
 * This file implements the MainWindow class which serves as the main UI for the TODO Widget
 * application. It manages the task list view, category filtering, system tray integration,
 * and coordinates between controllers and models.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QScreen>
#include <QSizeGrip>
#include "taskeditor.h"
#include "settingsdialog.h"
#include "timeentrydialog.h"
#include "timereportsdialog.h"
#include "../services/settingsmanager.h"

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
    : QMainWindow(parent), 
      m_isDragging(false)
{
    // Basic UI setup
    setupUi();

    setupModels();
    
    // Initialize the controllers with their corresponding models
    m_taskController = &TaskController::instance(m_taskModel);
    m_categoryController = &CategoryController::instance(m_categoryModel);
    
    setupSystemTray();
    
    // Now that we have the tray icon, set it in the notification controller
    m_notificationController = &NotificationController::instance(m_taskModel, m_trayIcon);
    
    m_timeTrackingController = &TimeTrackingController::instance(m_timeEntryModel);
    m_projectController = &ProjectController::instance(m_projectModel);
    
    setupConnections();
    loadSettings();

    // Initialize controllers - order matters here
    m_categoryController->loadCategories();
    
    // First load projects, then initialize time tracking controller
    m_projectController->loadProjects();
    m_timeTrackingController->initialize();
    
    // Now that projects and time entries are loaded, ensure the time tracker widget is updated
    m_timeTrackerWidget->updateProjectComboBox();
    
    m_taskController->loadTasks();

    // Start notification checking
    m_notificationController->start();
}

/**
 * @brief Destructor
 * 
 * Saves window settings before destruction.
 */
MainWindow::~MainWindow()
{
    saveSettings();
}

/**
 * @brief Show the window and bring it to front
 * 
 * If the window is minimized, restores it to normal state,
 * makes it visible, and brings it to the front.
 */
void MainWindow::showAndActivate()
{
    // If window is minimized, restore it
    if (isMinimized()) {
        showNormal();
    } else {
        show();
    }

    // Bring window to front
    activateWindow();
    raise();
}


/**
 * @brief Handle close event
 * 
 * Overrides the default close event behavior to minimize to system tray
 * instead of closing the application when the close button is clicked.
 * 
 * @param event The close event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

/**
 * @brief Handle mouse press event
 * 
 * Used for implementing window dragging since the window is frameless.
 * Starts dragging when the left mouse button is pressed.
 * 
 * @param event The mouse press event
 */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

/**
 * @brief Handle mouse move event
 * 
 * Used for implementing window dragging since the window is frameless.
 * Moves the window when the left mouse button is pressed and dragged.
 * 
 * @param event The mouse move event
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_isDragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

/**
 * @brief Handle mouse release event
 * 
 * Used for implementing window dragging since the window is frameless.
 * Stops dragging when the left mouse button is released.
 * 
 * @param event The mouse release event
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
}

/**
 * @brief Delayed initialization
 * 
 * Performs initialization operations that should be performed
 * after a delay to improve application startup time.
 */
void MainWindow::delayedInitialization()
{
    setupModels();
    setupConnections();
    setupSystemTray();
    loadSettings();
}

/**
 * @brief Set up the user interface
 * 
 * Creates and configures all UI elements including window properties,
 * layouts, controls, and connections.
 */
void MainWindow::setupUi()
{
    // Set window properties
    setWindowTitle("TODO Widget");
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(400, 600);

    setMinimumSize(300, 400);

    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    centralWidget->setStyleSheet("QWidget#centralWidget { background-color: rgba(255, 255, 255, 220); border-radius: 10px; }");
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    // Create title bar
    QHBoxLayout *titleBarLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("TODO Widget", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    m_settingsButton = new QPushButton(this);
    m_settingsButton->setIcon(QIcon(":/icons/settings.svg"));
    m_settingsButton->setFlat(true);
    m_settingsButton->setToolTip("Settings");
    m_settingsButton->setFixedSize(24, 24);

    QPushButton *closeButton = new QPushButton(this);
    closeButton->setIcon(QIcon(":/icons/close.svg"));
    closeButton->setFlat(true);
    closeButton->setToolTip("Close");
    closeButton->setFixedSize(24, 24);

    titleBarLayout->addWidget(titleLabel);
    titleBarLayout->addStretch();
    titleBarLayout->addWidget(m_settingsButton);
    titleBarLayout->addWidget(closeButton);

    mainLayout->addLayout(titleBarLayout);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget, 1);

    // Create task tab
    QWidget *taskTab = new QWidget(this);
    QVBoxLayout *taskLayout = new QVBoxLayout(taskTab);
    taskLayout->setContentsMargins(0, 0, 0, 0);
    taskLayout->setSpacing(5);

    // Create category filter
    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Category:", this);
    m_categoryFilterCombo = new QComboBox(this);

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(m_categoryFilterCombo, 1);

    taskLayout->addLayout(filterLayout);

    // Create task list view
    m_taskListView = new TaskListView(this);
    m_taskListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_taskListView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_taskListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    taskLayout->addWidget(m_taskListView, 1);

    // Create quick add layout
    QHBoxLayout *quickAddLayout = new QHBoxLayout();
    m_quickAddEdit = new QLineEdit(this);
    m_quickAddEdit->setPlaceholderText("Add a new task...");

    m_addTaskButton = new QPushButton(this);
    m_addTaskButton->setIcon(QIcon(":/icons/add.svg"));
    m_addTaskButton->setToolTip("Add Task");
    m_addTaskButton->setFixedSize(24, 24);

    quickAddLayout->addWidget(m_quickAddEdit);
    quickAddLayout->addWidget(m_addTaskButton);

    taskLayout->addLayout(quickAddLayout);

    // Add the task tab to the tab widget
    m_tabWidget->addTab(taskTab, "Tasks");

    // Create time tracking tab
    m_timeTrackerWidget = new TimeTrackerWidget(this);
    m_tabWidget->addTab(m_timeTrackerWidget, "Time Tracking");

    // Add a size grip to the bottom-right corner
    QSizeGrip *sizeGrip = new QSizeGrip(this);
    
    // Create a layout for the size grip
    QHBoxLayout *gripLayout = new QHBoxLayout();
    gripLayout->setContentsMargins(0, 0, 0, 0);
    gripLayout->addStretch();
    gripLayout->addWidget(sizeGrip);
    
    // Add the grip layout to the main layout
    mainLayout->addLayout(gripLayout);

    // Connect close button
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::hide);
}

/**
 * @brief Set up data models
 * 
 * Creates and initializes the task, category, project, and time entry models,
 * and configures the task list view with the appropriate model and delegate.
 */
void MainWindow::setupModels()
{
    m_taskModel = new TaskModel(this);
    m_categoryModel = new CategoryModel(this);
    m_timeEntryModel = new TimeEntryModel(this); //TimeTrackingController::instance().timeEntryModel();
    m_projectModel = new ProjectModel(this);

    // Set up task list view with model and delegate
    m_taskListView->setModel(m_taskModel);
    TaskItemDelegate *delegate = new TaskItemDelegate(m_categoryModel, this);
    m_taskListView->setItemDelegate(delegate);
}

/**
 * @brief Set up signal-slot connections
 * 
 * Establishes connections between UI elements, models, and controllers
 * to handle user interactions and data updates.
 */
void MainWindow::setupConnections()
{
    // Connect UI signals
    connect(m_addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(m_quickAddEdit, &QLineEdit::returnPressed, this, &MainWindow::onAddTaskClicked);
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(m_taskListView, &QListView::doubleClicked, this, &MainWindow::onTaskDoubleClicked);
    connect(m_taskListView, &QListView::customContextMenuRequested, this, &MainWindow::onTaskListContextMenu);
    connect(m_categoryFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCategoryFilterChanged);
    connect(m_categoryController, &CategoryController::categoriesChanged, this, &MainWindow::populateCategoryFilter);


    // Connect to the model's rowsMoved signal to handle task reordering
    connect(m_taskListView, &TaskListView::itemDropped,
                this, &MainWindow::onTaskDropped);

    // Connect controller signals
    connect(m_categoryController, &CategoryController::categoriesChanged, this, &MainWindow::populateCategoryFilter);
    
    // Connect project controller signals to update time tracking widgets
    // Connect all the relevant signals for project changes to ensure the TimeTrackerWidget is updated
    connect(m_projectController, &ProjectController::projectAdded, 
            m_timeTrackerWidget, &TimeTrackerWidget::updateProjectComboBox);
    connect(m_projectController, &ProjectController::projectUpdated, 
            m_timeTrackerWidget, &TimeTrackerWidget::updateProjectComboBox);
    connect(m_projectController, &ProjectController::projectDeleted, 
            m_timeTrackerWidget, &TimeTrackerWidget::updateProjectComboBox);
    connect(m_projectController, &ProjectController::projectsChanged, 
            m_timeTrackerWidget, &TimeTrackerWidget::updateProjectComboBox);
}

/**
 * @brief Set up system tray integration
 * 
 * Creates and configures the system tray icon and its context menu.
 * Allows the application to run in the background and be accessed from the system tray.
 */
void MainWindow::setupSystemTray()
{
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/app_icon.svg"), this);
    m_trayMenu = new QMenu(this);

    QAction *showHideAction = m_trayMenu->addAction("Show/Hide");
    connect(showHideAction, &QAction::triggered, this, &MainWindow::toggleVisibility);

    m_trayMenu->addSeparator();

    QAction *exitAction = m_trayMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExitActionTriggered);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->setToolTip("TODO Widget");
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
}

/**
 * @brief Load application settings
 * 
 * Retrieves window position, size, and other settings from the
 * SettingsManager and applies them to the UI. Ensures the window
 * is visible on screen and applies window flags and opacity.
 */
void MainWindow::loadSettings()
{
    // Load window position and size
    QPoint pos = SettingsManager::instance().windowPosition();
    QSize size = SettingsManager::instance().windowSize();

    // Ensure the window is visible on screen
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    if (!screenGeometry.contains(QRect(pos, size))) {
        pos = QPoint(
            (screenGeometry.width() - size.width()) / 2,
            (screenGeometry.height() - size.height()) / 2
        );
    }

    move(pos);
    resize(size);

    // Apply window flags and opacity
    updateWindowFlags();
    updateWindowOpacity();
}

/**
 * @brief Save application settings
 * 
 * Saves the current window position and size to the SettingsManager.
 * Called automatically when the window is closed or the application exits.
 */
void MainWindow::saveSettings()
{
    // Save window position and size
    SettingsManager::instance().setWindowPosition(pos());
    SettingsManager::instance().setWindowSize(size());
}

/**
 * @brief Update window flags
 * 
 * Updates the window flags based on current settings,
 * particularly whether the window should always stay on top.
 * Reapplies the flags and shows the window again.
 */
void MainWindow::updateWindowFlags()
{
    Qt::WindowFlags flags = Qt::FramelessWindowHint;

    if (SettingsManager::instance().alwaysOnTop()) {
        flags |= Qt::WindowStaysOnTopHint;
    }

    setWindowFlags(flags);
    show(); // Need to show again after changing flags
}

/**
 * @brief Update window opacity
 * 
 * Sets the window opacity level based on the value stored in settings.
 * This allows for customizable transparency of the application window.
 */
void MainWindow::updateWindowOpacity()
{
    double opacity = SettingsManager::instance().opacity();
    setWindowOpacity(opacity);
}

/**
 * @brief Populate category filter dropdown
 * 
 * Fills the category filter dropdown with available categories.
 * Preserves the current selection if possible and adds a color
 * indicator for each category. Includes an "All Categories" option
 * to view all tasks regardless of category.
 */
void MainWindow::populateCategoryFilter()
{
    // Store current selection
    QString currentCategoryId;
    if (m_categoryFilterCombo->currentIndex() >= 0) {
        currentCategoryId = m_categoryFilterCombo->currentData().toString();
    }
    
    m_categoryFilterCombo->blockSignals(true);
    m_categoryFilterCombo->clear();

    // Add "All Categories" option
    m_categoryFilterCombo->addItem("All Categories", "");

    // Add all categories
    QList<Category> categories = m_categoryModel->getCategories();
    for (const Category &category : categories) {
        QPixmap pixmap(16, 16);
        pixmap.fill(category.color());

        m_categoryFilterCombo->addItem(QIcon(pixmap), category.name(), category.id());
    }
    
    // Restore previous selection if it still exists
    if (!currentCategoryId.isEmpty()) {
        int index = m_categoryFilterCombo->findData(currentCategoryId);
        if (index >= 0) {
            m_categoryFilterCombo->setCurrentIndex(index);
        } else {
            // If the previously selected category was deleted, switch to "All Categories"
            m_categoryFilterCombo->setCurrentIndex(0);
        }
    }

    m_categoryFilterCombo->blockSignals(false);
    
    // If the selection changed while signals were blocked, manually trigger the filter
    if (m_categoryFilterCombo->currentData().toString() != currentCategoryId) {
        onCategoryFilterChanged(m_categoryFilterCombo->currentIndex());
    }
}

/**
 * @brief Toggle window visibility
 * 
 * Shows or hides the main window based on its current visibility state.
 * Used primarily for the system tray icon's show/hide action.
 * When showing, also activates the window to bring it to front.
 */
void MainWindow::toggleVisibility()
{
    if (isVisible()) {
        hide();
    } else {
        show();
        activateWindow();
    }
}

/**
 * @brief Handle add task button click or enter key in quick add field
 * 
 * Creates a new task with the text from the quick add edit field.
 * Assigns the default category or the first available category if no default is set.
 * Clears the input field after successful task creation.
 */
void MainWindow::onAddTaskClicked()
{
    QString title = m_quickAddEdit->text().trimmed();
    if (title.isEmpty()) {
        return;
    }

    // Get default category ID or use the first category if none is set
    QString categoryId = SettingsManager::instance().defaultCategoryId();
    if (categoryId.isEmpty() && m_categoryModel->rowCount() > 0) {
        categoryId = m_categoryModel->data(m_categoryModel->index(0, 0), CategoryModel::IdRole).toString();
    }

    if (m_taskController->addTask(title, categoryId)) {
        m_quickAddEdit->clear();
    }
}

/**
 * @brief Handle edit task action
 * 
 * Opens the task editor for the currently selected task.
 * Used when the edit action is triggered from the context menu.
 * Delegates to onTaskDoubleClicked for the actual editing logic.
 */
void MainWindow::onEditTaskClicked()
{
    QModelIndex currentIndex = m_taskListView->currentIndex();
    if (currentIndex.isValid()) {
        onTaskDoubleClicked(currentIndex);
    }
}

/**
 * @brief Handle delete task action
 * 
 * Deletes the currently selected task after confirmation.
 * Shows a confirmation dialog with the task title to prevent accidental deletion.
 * Used when the delete action is triggered from the context menu.
 */
void MainWindow::onDeleteTaskClicked()
{
    QModelIndex currentIndex = m_taskListView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }

    QString taskId = m_taskModel->data(currentIndex, TaskModel::IdRole).toString();
    QString taskTitle = m_taskModel->data(currentIndex, TaskModel::TitleRole).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete the task \"%1\"?").arg(taskTitle),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_taskController->deleteTask(taskId);
    }
}

/**
 * @brief Handle clear completed tasks action
 * 
 * Deletes all completed tasks after confirmation.
 * Shows a confirmation dialog to prevent accidental deletion.
 * Used when the "Clear Completed Tasks" action is triggered from the context menu.
 */
void MainWindow::onClearCompletedClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        "Are you sure you want to delete all completed tasks?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_taskController->deleteCompletedTasks();
    }
}

/**
 * @brief Handle task double-click
 * 
 * Opens the task editor dialog when a task is double-clicked.
 * Retrieves the task data, populates the editor, and applies updates
 * if the dialog is accepted. Used for editing existing tasks.
 * 
 * @param index Model index of the clicked task
 */
void MainWindow::onTaskDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    QString taskId = m_taskModel->data(index, TaskModel::IdRole).toString();
    Task task = m_taskModel->getTask(taskId);

    TaskEditor editor(m_categoryModel, this);
    editor.setTask(task);

    if (editor.exec() == QDialog::Accepted) {
        Task editedTask = editor.task();
        m_taskController->updateTask(
            editedTask.id(),
            editedTask.title(),
            editedTask.categoryId(),
            editedTask.description(),
            editedTask.dueDate(),
            editedTask.priority()
        );
    }
}

/**
 * @brief Handle category filter change
 * 
 * Updates the task list to show only tasks in the selected category.
 * If "All Categories" is selected, clears the filter to show all tasks.
 * 
 * @param index Index of the selected category in the dropdown
 */
void MainWindow::onCategoryFilterChanged(int index)
{
    QString categoryId = m_categoryFilterCombo->itemData(index).toString();

    if (categoryId.isEmpty()) {
        m_taskController->clearFilter();
    } else {
        m_taskController->filterByCategory(categoryId);
    }
}

/**
 * @brief Handle settings button click
 * 
 * Opens the settings dialog and applies any changes if the dialog is accepted.
 * Updates window flags, opacity, and notification settings based on the new values.
 * Used when the settings button in the title bar is clicked.
 */
void MainWindow::onSettingsClicked()
{
    SettingsDialog dialog(
        m_categoryModel, 
        this
    );

    if (dialog.exec() == QDialog::Accepted) {
        // Update window settings
        updateWindowFlags();
        updateWindowOpacity();

        // Update notification settings
        if (SettingsManager::instance().enableNotifications()) {
            m_notificationController->start();
        } else {
            m_notificationController->stop();
        }
    }
}

/**
 * @brief Handle tray icon activation
 * 
 * Toggles window visibility when the tray icon is clicked.
 * Shows or hides the main window depending on its current state.
 * 
 * @param reason Type of activation (click, double-click, etc.)
 */
void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        toggleVisibility();
    }
}

/**
 * @brief Handle exit action from system tray menu
 * 
 * Quits the application when the exit action is triggered from the system tray menu.
 * This completely closes the application rather than just hiding the window.
 */
void MainWindow::onExitActionTriggered()
{
    QApplication::quit();
}

/**
 * @brief Handle task list context menu request
 * 
 * Shows a context menu with task management options when right-clicking
 * on the task list. The menu contains different options depending on
 * whether a task was right-clicked or empty space in the list.
 * 
 * @param pos Position where the context menu was requested
 */
void MainWindow::onTaskListContextMenu(const QPoint &pos)
{
    QModelIndex index = m_taskListView->indexAt(pos);

    QMenu contextMenu(this);

    // If a task was clicked, show task-specific options
    if (index.isValid()) {
        QString taskId = m_taskModel->data(index, TaskModel::IdRole).toString();
        bool isCompleted = m_taskModel->data(index, TaskModel::CompletedRole).toBool();

        QAction *toggleAction = contextMenu.addAction(isCompleted ? "Mark as Incomplete" : "Mark as Complete");
        connect(toggleAction, &QAction::triggered, [this, taskId]() {
            m_taskController->toggleTaskCompletion(taskId);
        });

        QAction *editAction = contextMenu.addAction("Edit Task");
        connect(editAction, &QAction::triggered, this, &MainWindow::onEditTaskClicked);

        QAction *deleteAction = contextMenu.addAction("Delete Task");
        connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteTaskClicked);

        contextMenu.addSeparator();
    }

    // General options always shown
    QAction *addAction = contextMenu.addAction("Add New Task");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddTaskWithDialog);
    /*connect(addAction, &QAction::triggered, [this]() {
        m_quickAddEdit->setFocus();
    });*/

    QAction *clearCompletedAction = contextMenu.addAction("Clear Completed Tasks");
    connect(clearCompletedAction, &QAction::triggered, this, &MainWindow::onClearCompletedClicked);

    contextMenu.exec(m_taskListView->mapToGlobal(pos));
}

/**
 * @brief Handle add task with dialog
 * 
 * Opens the task editor dialog to create a new task with full details.
 * Provides a more comprehensive task creation experience than the quick add field.
 * Used when the "Add New Task" action is triggered from the context menu.
 */
void MainWindow::onAddTaskWithDialog()
{
    // Create a new empty task
    Task newTask;

    // Get default category ID or use the first category if none is set
    QString categoryId = SettingsManager::instance().defaultCategoryId();
    if (categoryId.isEmpty() && m_categoryModel->rowCount() > 0) {
        categoryId = m_categoryModel->data(m_categoryModel->index(0, 0), CategoryModel::IdRole).toString();
    }

    newTask.setCategoryId(categoryId);

    // Open the task editor
    TaskEditor editor(m_categoryModel, this);
    editor.setTask(newTask);
    editor.setWindowTitle("Add New Task");

    if (editor.exec() == QDialog::Accepted) {
        Task task = editor.task();
        m_taskController->addTask(
            task.title(),
            task.categoryId(),
            task.description(),
            task.dueDate(),
            task.priority()
        );
    }
}

/**
 * @brief Handle task reordering via model signal
 * 
 * Updates the task order when tasks are reordered via the model's rowsMoved signal.
 * Delegates to the task controller to handle the actual reordering logic.
 * 
 * @param parent Parent model index (unused in list models)
 * @param start Start row of the moved task
 * @param end End row of the moved task (same as start for single item moves)
 * @param destination Destination model index (unused in list models)
 * @param row Destination row where the task was moved
 */
void MainWindow::onTasksReordered(const QModelIndex &parent, int start, int end, 
    const QModelIndex &destination, int row)
{
    //Q_UNUSED(parent);
    //Q_UNUSED(end);
    //Q_UNUSED(destination);

    qDebug() << "Tasks reordered: start=" << start << ", end=" << end << ", destination=" << destination << ", row=" << row;

    // Call the controller to handle the reordering
    m_taskController->moveTask(start, row);
}

/**
 * @brief Handle task drop event
 * 
 * Updates the task order when a task is dropped into a new position.
 * Delegates to the task controller to handle the actual reordering logic.
 * 
 * @param sourceRow Original row of the dragged task
 * @param targetRow Target row where the task was dropped
 */
void MainWindow::onTaskDropped(int sourceRow, int targetRow)
{
    qDebug() << "Task dropped: from=" << sourceRow << ", to=" << targetRow;

    // Call the controller to handle the reordering
    m_taskController->moveTask(sourceRow, targetRow);
}
