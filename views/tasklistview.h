/**
 * @file tasklistview.h
 * @brief Definition of the TaskListView class
 * 
 * This file defines the TaskListView class which extends QListView to provide
 * custom drag-and-drop functionality for reordering tasks.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QListView>
#include <QDropEvent>

/**
 * @class TaskListView
 * @brief Custom list view for displaying and reordering tasks
 * 
 * The TaskListView class extends QListView to provide enhanced functionality
 * for the tasks list, particularly custom drag-and-drop handling for task
 * reordering. It emits a custom signal when tasks are reordered to allow
 * for proper model updates.
 */
class TaskListView : public QListView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new TaskListView with drag-and-drop enabled.
     * 
     * @param parent Optional parent widget
     */
    explicit TaskListView(QWidget *parent = nullptr);

signals:
    /**
     * @brief Signal emitted when a task is dropped after dragging
     * 
     * This signal is emitted when a drag-and-drop operation completes,
     * providing the source and target rows for the operation.
     * 
     * @param sourceRow The original row index of the dragged task
     * @param targetRow The destination row index where the task was dropped
     */
    void itemDropped(int sourceRow, int targetRow);

protected:
    /**
     * @brief Handle drop events
     * 
     * Overrides the QListView drop event handler to implement custom
     * task reordering logic and emit the itemDropped signal.
     * 
     * @param event The drop event object
     */
    void dropEvent(QDropEvent *event) override;
    
    /**
     * @brief Handle drag move events
     * 
     * Overrides the QListView drag move event handler to ensure
     * drop indicators are always shown during drag operations.
     * 
     * @param event The drag move event object
     */
    void dragMoveEvent(QDragMoveEvent *event) override;
};
