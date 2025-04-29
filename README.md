# TODO Widget

A lightweight, customizable desktop widget for managing to-do tasks built with Qt and C++.

## Project Overview

TODO Widget is a desktop application that provides a minimalist yet powerful interface for managing daily tasks. The application follows the Model-View-Controller (MVC) architectural pattern and supports features such as:

- Task categorization with color coding
- Due date reminders and notifications
- Task prioritization
- Desktop and system tray integration
- Customizable appearance
- Data persistence using SQLite
- Import/export functionality

## Project Structure

The project is organized into the following directories:

- **controllers/**: Business logic components that connect models and views
  - `categorycontroller`: Manages category operations
  - `notificationcontroller`: Handles due date notifications
  - `taskcontroller`: Manages task operations

- **models/**: Data structures and their associated logic
  - `category`: Represents a task category
  - `categorymodel`: List model for categories
  - `task`: Represents a single to-do item
  - `taskmodel`: List model for tasks

- **services/**: Utility services and managers
  - `databasemanager`: Handles database operations
  - `importexportservice`: Handles importing/exporting tasks
  - `settingsmanager`: Manages application settings

- **views/**: UI components and custom widgets
  - `categorydelegate`: Custom rendering for categories
  - `categoryeditor`: Dialog for editing categories
  - `mainwindow`: Main application window
  - `settingsdialog`: Application settings dialog
  - `taskeditor`: Dialog for editing tasks
  - `taskitemdelegate`: Custom rendering for tasks
  - `tasklistview`: Custom view for the task list

- **resources/**: Application resources
  - `icons/`: SVG icons for the application
  - `styles/`: QSS stylesheets

## Documentation Standards

The codebase follows these documentation standards:

1. **File Headers**: Each file includes a header comment with:
   - File name
   - Brief description
   - Author information
   - Last updated date

2. **Class Documentation**: Each class is documented with:
   - Brief description
   - Detailed explanation of purpose and functionality
   - Any important implementation details or usage notes

3. **Method Documentation**: Each method is documented with:
   - Brief description of its purpose
   - Parameter descriptions
   - Return value description
   - Any side effects or important notes

4. **Property Documentation**: Class member variables are documented with:
   - Brief inline comments explaining their purpose

5. **Doxygen Compatibility**: Documentation follows Doxygen syntax for automatic documentation generation.

## Building the Project

### Prerequisites

- Qt 5.12 or later
- C++11 compatible compiler

### Build Instructions

1. Open `TODO-Widget.pro` in Qt Creator
2. Configure the project for your target platform
3. Build the project

## License

GNU GPL v3

## Contributors

Cornebidouil
