# TODO Widget Architecture

This document describes the architectural design of the TODO Widget application.

## Overview

TODO Widget follows the Model-View-Controller (MVC) architectural pattern to separate concerns and create a maintainable, modular codebase. The application is built using Qt, which provides a rich set of UI components and cross-platform capabilities.

## Core Architectural Components

### Models

Models represent the application's data structures and business logic.

- **Task**: Represents a single to-do item with properties like title, description, due date, completion status, and priority.
- **Category**: Represents a group for organizing tasks with properties like name and color.
- **TaskModel**: A QAbstractListModel implementation that manages a collection of Task objects and provides methods for filtering and sorting.
- **CategoryModel**: A QAbstractListModel implementation that manages a collection of Category objects.

### Views

Views are responsible for displaying data to the user and handling user interactions.

- **MainWindow**: The primary window containing the task list, quick add field, and category filters.
- **TaskListView**: A custom QListView that displays tasks and supports drag-and-drop reordering.
- **TaskItemDelegate**: A custom QStyledItemDelegate that handles the rendering of tasks in the list view.
- **TaskEditor**: A dialog for creating and editing tasks.
- **CategoryEditor**: A dialog for creating and editing categories.
- **SettingsDialog**: A dialog for modifying application settings.
- **CategoryDelegate**: A custom QStyledItemDelegate for rendering categories in selection widgets.

### Controllers

Controllers connect models and views, handling user actions and updating the models/views accordingly.

- **TaskController**: Handles operations on tasks, such as adding, updating, deleting, and filtering.
- **CategoryController**: Handles operations on categories, such as adding, updating, and deleting.
- **NotificationController**: Monitors tasks with due dates and displays notifications when appropriate.

### Services

Services provide utility functions and manage application-wide resources.

- **DatabaseManager**: Handles database connections and CRUD operations for persistent storage.
- **SettingsManager**: Manages application settings and user preferences.
- **ImportExportService**: Provides functionality for importing and exporting tasks.

## Component Interactions

### Task Creation Flow

1. User enters a task title in the quick add field or opens the task editor dialog.
2. The MainWindow captures this action and delegates to the TaskController.
3. TaskController creates a new Task object and adds it to the TaskModel.
4. TaskModel emits signals that the view observes, updating the UI.
5. TaskController calls DatabaseManager to persist the new task.

### Task Filtering Flow

1. User selects a category from the filter dropdown.
2. MainWindow captures this action and delegates to the TaskController.
3. TaskController applies a filter to the TaskModel.
4. TaskModel updates its data and notifies the view.
5. TaskListView refreshes to show only tasks in the selected category.

### Notification Flow

1. NotificationController regularly checks tasks with due dates.
2. When a task is approaching or past its due date, NotificationController shows a system tray notification.
3. User can click the notification to view the task in the application.

## Data Persistence

The application uses SQLite for data persistence, managed through the DatabaseManager service.

- Tasks and categories are serialized to/from JSON for storage in the database.
- Database tables include:
  - `tasks`: Stores task data
  - `categories`: Stores category data
  - `settings`: Stores application settings

## Cross-Cutting Concerns

### Error Handling

- Database errors are logged and reported to the user when appropriate.
- Input validation occurs in controllers before data is passed to models.
- File I/O operations for import/export have proper error handling.

### Logging

- Application uses Qt's logging framework (QDebug, QInfo, etc.).
- A custom message handler redirects logs to a file in the application data directory.
- Logs include timestamps and contextual information based on severity.

### Settings Management

- User preferences are stored in the database and loaded at application startup.
- Settings include window position, opacity, startup behavior, notification preferences, etc.

## UI/UX Considerations

- The application can run with or without a window frame, depending on user preference.
- The window can be made semi-transparent for less visual interference.
- When closed, the application minimizes to the system tray rather than exiting.
- Tasks can be reordered via drag and drop.
- Tasks are color-coded based on their category.
- Quick add functionality allows rapid task creation.

## System Requirements

- Qt 5.12 or later
- C++11 compatible compiler
- Operating systems:
  - Windows 7 or later
  - macOS 10.12 or later
  - Linux with a compatible desktop environment

## Future Architecture Considerations

Potential future enhancements:

- Cloud synchronization service integration
- Task sharing functionality
- Plugin system for extensibility
- Mobile companion application
