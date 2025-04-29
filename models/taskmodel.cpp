/**
 * @file taskmodel.cpp
 * @brief Implementation of the TaskModel class
 * 
 * This file implements the TaskModel class which manages a collection of Task objects
 * and implements the Qt Model/View architecture. It provides functionality for
 * task management, filtering, sorting, and drag-and-drop reordering.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "taskmodel.h"
#include <algorithm>
#include <QMimeData>
#include <QDataStream>
#include <QDebug>

/**
 * @brief Constructor
 * 
 * Creates a new empty TaskModel with filtering disabled.
 * 
 * @param parent Optional parent QObject
 */
TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel(parent), m_isFiltered(false)
{
}

/**
 * @brief Get the number of rows (tasks) in the model
 * 
 * Returns the number of tasks currently visible in the model,
 * which depends on whether filtering is active.
 * 
 * @param parent Parent model index (unused in list models)
 * @return int Number of tasks in the model
 */
int TaskModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_isFiltered ? m_filteredTasks.size() : m_tasks.size();
}

/**
 * @brief Get data for a specific task and role
 * 
 * Returns the requested data from the task at the specified index.
 * The data returned depends on the role requested.
 * 
 * @param index Model index identifying the task
 * @param role Data role to retrieve
 * @return QVariant The requested data
 */
QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // Get the appropriate task based on whether filtering is active
    const Task &task = m_isFiltered ? m_filteredTasks[index.row()] : m_tasks[index.row()];

    // Return the requested data based on the role
    switch (role) {
        case Qt::DisplayRole:
        case TitleRole:
            return task.title();
        case DescriptionRole:
            return task.description();
        case CompletedRole:
            return task.isCompleted();
        case CreatedDateRole:
            return task.createdDate();
        case DueDateRole:
            return task.dueDate();
        case CategoryIdRole:
            return task.categoryId();
        case PriorityRole:
            return task.priority();
        case IdRole:
            return task.id();
        case DisplayOrderRole:
            return task.displayOrder();
        default:
            return QVariant();
    }
}

/**
 * @brief Set data for a specific task and role
 * 
 * Updates the specified data in the task at the given index.
 * The data updated depends on the role specified.
 * If filtering is active, the change is also propagated to the main task list.
 * 
 * @param index Model index identifying the task
 * @param value New value to set
 * @param role Data role to modify
 * @return bool True if data was successfully set, false otherwise
 */
bool TaskModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    // Get the appropriate task reference based on whether filtering is active
    Task &task = m_isFiltered ? m_filteredTasks[index.row()] : m_tasks[index.row()];

    // Update the requested data based on the role
    switch (role) {
        case TitleRole:
            task.setTitle(value.toString());
            break;
        case DescriptionRole:
            task.setDescription(value.toString());
            break;
        case CompletedRole:
            task.setCompleted(value.toBool());
            break;
        case DueDateRole:
            task.setDueDate(value.toDateTime());
            break;
        case CategoryIdRole:
            task.setCategoryId(value.toString());
            break;
        case PriorityRole:
            task.setPriority(value.toInt());
            break;
        case DisplayOrderRole:
            task.setDisplayOrder(value.toInt());
            break;
        default:
            return false;
    }

    // If we're working with a filtered list, update the main list too
    if (m_isFiltered) {
        for (int i = 0; i < m_tasks.size(); ++i) {
            if (m_tasks[i].id() == task.id()) {
                m_tasks[i] = task;
                break;
            }
        }
    }

    // Notify views that the data has changed
    emit dataChanged(index, index, {role});
    return true;
}

/**
 * @brief Get item flags for a specific index
 * 
 * Returns the flags for the task at the specified index.
 * All tasks are enabled, selectable, editable, and support drag and drop.
 * 
 * @param index Model index to get flags for
 * @return Qt::ItemFlags Flags for the specified task
 */
Qt::ItemFlags TaskModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

/**
 * @brief Get role names used for QML integration
 * 
 * Returns a mapping from role values to role names for use in QML.
 * 
 * @return QHash<int, QByteArray> Mapping from role values to role names
 */
QHash<int, QByteArray> TaskModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[CompletedRole] = "completed";
    roles[CreatedDateRole] = "createdDate";
    roles[DueDateRole] = "dueDate";
    roles[CategoryIdRole] = "categoryId";
    roles[PriorityRole] = "priority";
    roles[DisplayOrderRole] = "displayOrder";
    return roles;
}

/**
 * @brief Get supported drop actions
 * 
 * Returns the drop actions supported by this model.
 * Only move operations are supported for task reordering.
 * 
 * @return Qt::DropActions Supported drop actions
 */
Qt::DropActions TaskModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

/**
 * @brief Check if data can be dropped at the specified position
 * 
 * Verifies that the drop operation is valid for this model.
 * Only allows internal moves of task items.
 * 
 * @param data MIME data being dropped
 * @param action Drop action being performed
 * @param row Target row
 * @param column Target column
 * @param parent Parent model index
 * @return bool True if the data can be dropped here
 */
bool TaskModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    
    // Only allow internal moves with the appropriate MIME type
    return action == Qt::MoveAction && data->hasFormat("application/x-qabstractitemmodeldatalist");
}

/**
 * @brief Handle dropping data at the specified position
 * 
 * Processes a drop operation for task reordering.
 * Extracts the source row from the MIME data and calls moveTask.
 * 
 * @param data MIME data being dropped
 * @param action Drop action being performed
 * @param row Target row
 * @param column Target column
 * @param parent Parent model index
 * @return bool True if the drop was successful
 */
bool TaskModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;
        
    // If row is -1, drop at the end
    if (row == -1)
        row = rowCount(QModelIndex());
        
    // Get source row from mime data
    QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    int sourceRow;
    stream >> sourceRow;
    
    // Move the task from the source row to the target row
    return moveTask(sourceRow, row);
}

/**
 * @brief Create MIME data for dragging tasks
 * 
 * Creates MIME data containing the row of the first dragged task.
 * 
 * @param indexes List of indexes being dragged
 * @return QMimeData* MIME data representing the dragged tasks
 */
QMimeData *TaskModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    
    // Store the row of the first index
    if (!indexes.isEmpty()) {
        stream << indexes.first().row();
    }
    
    mimeData->setData("application/x-qabstractitemmodeldatalist", encoded);
    return mimeData;
}

/**
 * @brief Get supported MIME types for drag and drop
 * 
 * Returns the list of MIME types supported for drag and drop operations.
 * 
 * @return QStringList List of supported MIME types
 */
QStringList TaskModel::mimeTypes() const
{
    return QStringList() << "application/x-qabstractitemmodeldatalist";
}

/**
 * @brief Add a task to the model
 * 
 * Adds a new task to the model, assigning it the next available display order.
 * If filtering is active, the task is added to the filtered list as well
 * if it matches the current filter.
 * 
 * @param task The task to add
 */
void TaskModel::addTask(const Task &task)
{
    // Create a copy of the task to modify
    Task newTask = task;
    
    // Set display order to the next available value
    newTask.setDisplayOrder(getNextDisplayOrder());
    
    // Add to the main task list
    beginInsertRows(QModelIndex(), m_tasks.size(), m_tasks.size());
    m_tasks.append(newTask);
    endInsertRows();

    // If filtering is active and the task matches the filter, add to filtered list too
    if (m_isFiltered && !m_filteredTasks.isEmpty() && newTask.categoryId() == m_filteredTasks.first().categoryId()) {
        beginInsertRows(QModelIndex(), m_filteredTasks.size(), m_filteredTasks.size());
        m_filteredTasks.append(newTask);
        endInsertRows();
    }
}

/**
 * @brief Remove a task from the model
 * 
 * Removes the task with the specified ID from the model.
 * If filtering is active, the task is removed from the filtered list as well.
 * Updates display orders after removal.
 * 
 * @param id ID of the task to remove
 * @return bool True if the task was found and removed
 */
bool TaskModel::removeTask(const QString &id)
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id() == id) {
            // If we're filtering, remove from filtered list too
            if (m_isFiltered) {
                for (int j = 0; j < m_filteredTasks.size(); ++j) {
                    if (m_filteredTasks[j].id() == id) {
                        beginRemoveRows(QModelIndex(), j, j);
                        m_filteredTasks.removeAt(j);
                        endRemoveRows();
                        break;
                    }
                }
            }

            // Remove from the main list
            beginRemoveRows(QModelIndex(), i, i);
            m_tasks.removeAt(i);
            endRemoveRows();

            // Update display orders after removal
            updateDisplayOrders();
            return true;
        }
    }
    return false;
}

/**
 * @brief Get a task by ID
 * 
 * Retrieves the task with the specified ID.
 * 
 * @param id ID of the task to retrieve
 * @return Task The requested task, or an empty task if not found
 */
Task TaskModel::getTask(const QString &id) const
{
    for (const Task &task : m_tasks) {
        if (task.id() == id) {
            return task;
        }
    }
    return Task(); // Return empty task if not found
}

/**
 * @brief Get all tasks in the model
 * 
 * Returns the complete list of tasks, regardless of filtering.
 * 
 * @return QList<Task> List of all tasks
 */
QList<Task> TaskModel::getTasks() const
{
    return m_tasks;
}

/**
 * @brief Replace all tasks in the model
 * 
 * Replaces the current task list with a new one and clears any filtering.
 * Ensures all tasks have valid display orders.
 * 
 * @param tasks New list of tasks
 */
void TaskModel::setTasks(const QList<Task> &tasks)
{
    beginResetModel();
    m_tasks = tasks;
    m_isFiltered = false;
    
    // Ensure all tasks have a display order
    bool needsOrdering = false;
    for (const Task &task : m_tasks) {
        if (task.displayOrder() < 0) {
            needsOrdering = true;
            break;
        }
    }
    
    if (needsOrdering) {
        updateDisplayOrders();
    }
    
    endResetModel();
}

/**
 * @brief Move a task from one position to another
 * 
 * Moves a task from the source row to the target row, updating the model
 * and the task display orders. If filtering is active, the changes are
 * propagated to the main task list as well.
 * 
 * @param fromRow Source row
 * @param toRow Destination row
 * @return bool True if the move was successful
 */
bool TaskModel::moveTask(int fromRow, int toRow)
{
    int rowCount = this->rowCount();

    // Validate row indices
    if (fromRow < 0 || fromRow >= rowCount) {
        qDebug() << "Invalid source row:" << fromRow;
        return false;
    }

    // Allow toRow to be equal to rowCount to indicate "move to end"
    if (toRow < 0 || toRow > rowCount) {
        qDebug() << "Invalid destination row:" << toRow;
        return false;
    }

    // If source and destination are the same, no move needed
    if (fromRow == toRow) {
        return true;
    }

    qDebug() << "Moving task in model from" << fromRow << "to" << toRow;

    // Perform the move operation
    beginMoveRows(QModelIndex(), fromRow, fromRow, QModelIndex(), toRow > fromRow ? toRow + 1 : toRow);

    QList<Task> &taskList = m_isFiltered ? m_filteredTasks : m_tasks;
    taskList.move(fromRow, toRow);

    endMoveRows();

    // Update display orders after move
    updateDisplayOrders();

    // If we're working with a filtered list, update the main list too
    if (m_isFiltered) {
        // Map filtered tasks back to main tasks by ID
        for (int i = 0; i < m_filteredTasks.size(); ++i) {
            const Task &filteredTask = m_filteredTasks[i];
            for (int j = 0; j < m_tasks.size(); ++j) {
                if (m_tasks[j].id() == filteredTask.id()) {
                    m_tasks[j].setDisplayOrder(filteredTask.displayOrder());
                    break;
                }
            }
        }
    }

    return true;
}

/**
 * @brief Update display orders for all tasks
 * 
 * Ensures all tasks have sequential display orders based on their
 * current position in the list. If filtering is active, the changes
 * are propagated to the main task list as well.
 */
void TaskModel::updateDisplayOrders()
{
    QList<Task> &taskList = m_isFiltered ? m_filteredTasks : m_tasks;
    
    // Update display orders based on position in the list
    for (int i = 0; i < taskList.size(); ++i) {
        taskList[i].setDisplayOrder(i);
        
        // Emit data changed signal
        QModelIndex index = this->index(i, 0);
        emit dataChanged(index, index, {DisplayOrderRole});
    }
    
    // If we're working with a filtered list, update the main list too
    if (m_isFiltered) {
        for (const Task &filteredTask : m_filteredTasks) {
            for (int i = 0; i < m_tasks.size(); ++i) {
                if (m_tasks[i].id() == filteredTask.id()) {
                    m_tasks[i].setDisplayOrder(filteredTask.displayOrder());
                    break;
                }
            }
        }
    }
}

/**
 * @brief Filter tasks by category
 * 
 * Shows only tasks belonging to the specified category.
 * If categoryId is empty, clears the filter.
 * 
 * @param categoryId ID of the category to filter by
 */
void TaskModel::filterByCategory(const QString &categoryId)
{
    beginResetModel();

    if (categoryId.isEmpty()) {
        m_isFiltered = false;
    } else {
        m_filteredTasks.clear();
        for (const Task &task : m_tasks) {
            if (task.categoryId() == categoryId) {
                m_filteredTasks.append(task);
            }
        }
        m_isFiltered = true;
    }

    endResetModel();
}

/**
 * @brief Clear any active filters
 * 
 * Shows all tasks regardless of category.
 */
void TaskModel::clearFilter()
{
    if (m_isFiltered) {
        beginResetModel();
        m_isFiltered = false;
        endResetModel();
    }
}

/**
 * @brief Sort tasks by due date
 * 
 * Sorts tasks by their due dates, with tasks without due dates
 * going to the end. Updates display orders after sorting.
 * 
 * @param ascending If true, sorts from earliest to latest; if false, from latest to earliest
 */
void TaskModel::sortByDueDate(bool ascending)
{
    beginResetModel();

    auto comparator = [ascending](const Task &a, const Task &b) {
        // Tasks without due dates go to the end
        if (!a.dueDate().isValid() && b.dueDate().isValid())
            return false;
        if (a.dueDate().isValid() && !b.dueDate().isValid())
            return true;
        if (!a.dueDate().isValid() && !b.dueDate().isValid())
            return false;

        return ascending ? (a.dueDate() < b.dueDate()) : (a.dueDate() > b.dueDate());
    };

    // Sort the appropriate list
    if (m_isFiltered) {
        std::sort(m_filteredTasks.begin(), m_filteredTasks.end(), comparator);
    } else {
        std::sort(m_tasks.begin(), m_tasks.end(), comparator);
    }
    
    // Update display orders after sorting
    updateDisplayOrders();

    endResetModel();
}

/**
 * @brief Sort tasks by priority
 * 
 * Sorts tasks by their priority levels. Updates display orders after sorting.
 * 
 * @param ascending If true, sorts from lowest to highest; if false, from highest to lowest
 */
void TaskModel::sortByPriority(bool ascending)
{
    beginResetModel();

    auto comparator = [ascending](const Task &a, const Task &b) {
        return ascending ? (a.priority() < b.priority()) : (a.priority() > b.priority());
    };

    // Sort the appropriate list
    if (m_isFiltered) {
        std::sort(m_filteredTasks.begin(), m_filteredTasks.end(), comparator);
    } else {
        std::sort(m_tasks.begin(), m_tasks.end(), comparator);
    }
    
    // Update display orders after sorting
    updateDisplayOrders();

    endResetModel();
}

/**
 * @brief Get the next available display order
 * 
 * Finds the highest current display order and returns the next value.
 * 
 * @return int The next available display order value
 */
int TaskModel::getNextDisplayOrder() const
{
    int maxOrder = -1;
    
    for (const Task &task : m_tasks) {
        if (task.displayOrder() > maxOrder) {
            maxOrder = task.displayOrder();
        }
    }
    
    return maxOrder + 1;
}
