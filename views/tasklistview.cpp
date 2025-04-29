/**
 * @file tasklistview.cpp
 * @brief Implementation of the TaskListView class
 * 
 * This file implements the TaskListView class which provides a customized
 * QListView with enhanced drag-and-drop functionality for reordering tasks.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "tasklistview.h"
#include <QDebug>
#include <QMimeData>

/**
 * @brief Constructor implementation
 * 
 * Initializes the TaskListView with drag-and-drop functionality enabled
 * and configures a custom style for the drop indicator.
 * 
 * @param parent Optional parent widget
 */
TaskListView::TaskListView(QWidget *parent)
    : QListView(parent)
{
    // Enable drag and drop functionality
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);

    // Set custom style for drop indicator to show a horizontal line
    // This makes it more visually obvious where items will be dropped
    setStyleSheet(
        "QListView::drop-indicator {"
        "    background-color: #3080FF;"  // Blue indicator color
        "    height: 2px;"                // Thin line
        "    width: 100%;"                // Full width of the list
        "    margin: 0px;"                // No margins
        "}"
    );
}

/**
 * @brief Custom drop event handler
 * 
 * Overrides the default QListView drop event handling to implement
 * custom task reordering logic. This method:
 * 1. Extracts the source row from the mime data
 * 2. Determines the target row based on the drop position
 * 3. Emits the itemDropped signal with source and target rows
 * 4. Accepts the event to prevent default handling
 * 
 * @param event The drop event object
 */
void TaskListView::dropEvent(QDropEvent *event)
{
    // Get the source row from the mime data
    const QMimeData *mimeData = event->mimeData();
    QByteArray encoded = mimeData->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    int sourceRow;
    if (!stream.atEnd()) {
        // Extract row, column, and role data map from the stream
        int row, col;
        QMap<int, QVariant> roleDataMap;
        stream >> row >> col >> roleDataMap;
        sourceRow = row;
    } else {
        // If we can't get the source row, fall back to default handling
        QListView::dropEvent(event);
        return;
    }

    // Get the target row based on the drop index
    QModelIndex dropIndex = indexAt(event->pos());
    int targetRow;

    if (dropIndex.isValid()) {
        // If dropping on an item, use its row as the target
        // The dropped item will take this position, pushing the target item down
        targetRow = dropIndex.row();
    } else {
        // If dropping in an empty area, drop at the end of the list
        targetRow = model()->rowCount();
    }

    qDebug() << "Drop event: sourceRow=" << sourceRow << ", targetRow=" << targetRow;

    // Emit our custom signal to notify listeners about the reordering
    emit itemDropped(sourceRow, targetRow);

    // Accept the event to prevent the default handling
    // We'll handle the actual model updates in the controller
    event->acceptProposedAction();
}

/**
 * @brief Custom drag move event handler
 * 
 * Overrides the default QListView drag move event handling to ensure
 * the drop indicator is always shown during drag operations.
 * 
 * @param event The drag move event object
 */
void TaskListView::dragMoveEvent(QDragMoveEvent *event)
{
    // Get the index at the current position (not used directly but useful for debugging)
    QModelIndex index = indexAt(event->pos());

    // Always show the drop indicator
    setDropIndicatorShown(true);

    // Let the base class handle the standard event processing
    QListView::dragMoveEvent(event);
}
