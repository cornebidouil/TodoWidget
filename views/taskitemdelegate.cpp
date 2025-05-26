/**
 * @file taskitemdelegate.cpp
 * @brief Implementation of the TaskItemDelegate class
 * 
 * This file implements the TaskItemDelegate class which is responsible for
 * rendering task items in the task list view. It provides custom drawing 
 * of task elements including checkboxes, titles, due dates, priorities,
 * and category color indicators.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "taskitemdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QDateTime>
#include "../models/taskmodel.h"

/**
 * @brief Constructor
 * 
 * Creates a new TaskItemDelegate with the specified category model.
 * The category model is used to obtain category colors for tasks.
 * 
 * @param categoryModel Pointer to the CategoryModel to use for category information
 * @param parent Optional parent QObject
 */
TaskItemDelegate::TaskItemDelegate(CategoryModel *categoryModel, QObject *parent)
    : QStyledItemDelegate(parent), m_categoryModel(categoryModel)
{
}

/**
 * @brief Paint a task item
 * 
 * Renders a task item with custom styling including:
 * - A colored bar on the left indicating the task's category
 * - A checkbox for completion status
 * - The task title (with strikethrough if completed)
 * - Due date text (colored based on urgency)
 * - A priority indicator (colored circle)
 * 
 * @param painter The QPainter to use for drawing
 * @param option The style options for the item
 * @param index The model index of the task to paint
 */
void TaskItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    // Get task data
    QString title = index.data(TaskModel::TitleRole).toString();
    bool completed = index.data(TaskModel::CompletedRole).toBool();
    QDateTime dueDate = index.data(TaskModel::DueDateRole).toDateTime();
    QString categoryId = index.data(TaskModel::CategoryIdRole).toString();
    int priority = index.data(TaskModel::PriorityRole).toInt();

    // Get category color
    QColor categoryColor = Qt::gray;
    Category category = m_categoryModel->getCategory(categoryId);
    if (!category.id().isEmpty()) {
        categoryColor = category.color();
    }

    // Set up painter
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Draw selection background if selected
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color());
    } else {
        painter->setPen(option.palette.text().color());
    }

    // Draw category color bar
    QRect colorBarRect = option.rect;
    colorBarRect.setWidth(5);
    painter->fillRect(colorBarRect, categoryColor);

    // Draw checkbox
    QRect checkRect = checkboxRect(option);
    QStyleOptionButton checkboxOption;
    checkboxOption.rect = checkRect;
    checkboxOption.state = QStyle::State_Enabled;
    if (completed) {
        checkboxOption.state |= QStyle::State_On;
    } else {
        checkboxOption.state |= QStyle::State_Off;
    }
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxOption, painter);

    // Draw task title with strikethrough if completed
    QRect textRect = option.rect;
    textRect.setLeft(checkRect.right() + 5);
    textRect.setRight(textRect.right() - 5);

    QFont font = option.font;
    if (completed) {
        font.setStrikeOut(true);
        painter->setPen(Qt::gray);
    }
    painter->setFont(font);

    // Truncate text if needed
    QString elidedTitle = option.fontMetrics.elidedText(title, Qt::ElideRight, textRect.width());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedTitle);

    // Draw due date if available
    if (dueDate.isValid()) {
        QString dueDateText = dueDate.date().toString("MM/dd/yyyy");
        QRect dateRect = option.rect;
        dateRect.setLeft(textRect.right() - 80);
        dateRect.setRight(textRect.right());

        // Set color based on due date (red if overdue, orange if due today)
        QColor dateColor = Qt::black;
        QDateTime now = QDateTime::currentDateTime();
        if (dueDate < now) {
            dateColor = Qt::red;
        } else if (dueDate.date() == now.date()) {
            dateColor = QColor(255, 140, 0); // Orange
        }

        painter->setPen(dateColor);
        painter->drawText(dateRect, Qt::AlignRight | Qt::AlignVCenter, dueDateText);
    }

    // Draw priority indicator (small colored circle)
    if (priority > 0) {
        QRect priorityRect = option.rect;
        priorityRect.setLeft(option.rect.right() - 15);
        priorityRect.setWidth(10);
        priorityRect.setHeight(10);
        priorityRect.moveCenter(QPoint(priorityRect.center().x(), option.rect.center().y()));

        painter->setPen(Qt::NoPen);
        painter->setBrush(priorityColor(priority));
        painter->drawEllipse(priorityRect);
    }

    painter->restore();
}

/**
 * @brief Get the size hint for the task item
 * 
 * Returns the suggested size for a task item, ensuring a minimum height
 * to make items easier to interact with on touch devices and to provide
 * sufficient space for all visual elements.
 * 
 * @param option The style options for the item
 * @param index The model index of the task
 * @return QSize The suggested size for the item
 */
QSize TaskItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    return QSize(size.width(), qMax(size.height(), 40)); // Ensure minimum height
}

/**
 * @brief Handle editor events for the task item
 * 
 * Processes mouse events on task items, particularly clicks on the checkbox
 * to toggle the completion status of a task. This allows users to mark tasks
 * as complete or incomplete directly from the task list.
 * 
 * @param event The event to handle
 * @param model The model containing the task data
 * @param option The style options for the item
 * @param index The model index of the task
 * @return bool True if the event was handled, false otherwise
 */
bool TaskItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (!index.isValid() || event->type() != QEvent::MouseButtonRelease) {
        return false;
    }

    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    QRect checkRect = checkboxRect(option);

    if (checkRect.contains(mouseEvent->pos())) {
        // Get the task ID
        QString taskId = index.data(TaskModel::IdRole).toString();
        if (!taskId.isEmpty()) {
            // Use the controller to toggle and save the task completion status
            TaskController::instance().toggleTaskCompletion(taskId);
        }
        return true;
    }

    return false;
}

/**
 * @brief Calculate the checkbox rectangle
 * 
 * Determines the position and size of the checkbox within a task item.
 * This is used both for drawing the checkbox and for detecting clicks on it.
 * 
 * @param option The style options for the item
 * @return QRect The rectangle containing the checkbox
 */
QRect TaskItemDelegate::checkboxRect(const QStyleOptionViewItem &option) const
{
    QStyleOptionButton checkboxOption;
    QRect checkRect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxOption);
    checkRect.moveCenter(QPoint(checkRect.center().x() + 10, option.rect.center().y()));
    return checkRect;
}

/**
 * @brief Get the color for a priority level
 * 
 * Maps priority levels (1-5) to specific colors for visual indication.
 * Higher priority tasks have more vibrant colors to draw attention.
 * 
 * @param priority The priority level (1-5)
 * @return QColor The color corresponding to the priority level
 */
QColor TaskItemDelegate::priorityColor(int priority) const
{
    switch (priority) {
        case 1: return QColor(200, 200, 200); // Low - Gray
        case 2: return QColor(0, 150, 0);     // Normal-Low - Green
        case 3: return QColor(0, 0, 255);     // Normal - Blue
        case 4: return QColor(255, 140, 0);   // Normal-High - Orange
        case 5: return QColor(255, 0, 0);     // High - Red
        default: return Qt::transparent;
    }
}
