/**
 * @file taskitemdelegate.h
 * @brief Definition of the TaskItemDelegate class
 * 
 * This file defines the TaskItemDelegate class which customizes the rendering
 * of task items in list views. It provides visual enhancements such as category
 * colors, priority indicators, and interactive checkboxes.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QStyledItemDelegate>
#include "../models/categorymodel.h"
#include "../controllers/taskcontroller.h"
#include "../controllers/taskcontroller.h"

/**
 * @class TaskItemDelegate
 * @brief Custom delegate for rendering task items
 * 
 * The TaskItemDelegate class extends QStyledItemDelegate to provide a
 * customized rendering of task items in list views. It enhances the default
 * appearance with features such as:
 * - Color-coded category indicators
 * - Priority level visualization
 * - Interactive completion checkboxes
 * - Due date highlighting
 */
class TaskItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new TaskItemDelegate that uses the specified category model
     * to access category information for color coding.
     * 
     * @param categoryModel Pointer to the category model for color info
     * @param parent Optional parent QObject
     */
    explicit TaskItemDelegate(CategoryModel *categoryModel, QObject *parent = nullptr);

    /**
     * @brief Custom painting implementation
     * 
     * Overrides the default painting to implement custom rendering of task items.
     * 
     * @param painter The painter to use for drawing
     * @param option Style options for the item
     * @param index Model index of the item to paint
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
    /**
     * @brief Custom size hint implementation
     * 
     * Provides size information for task items.
     * 
     * @param option Style options for the item
     * @param index Model index of the item
     * @return QSize The recommended size for the item
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
    /**
     * @brief Custom editor event handler
     * 
     * Handles editor events such as mouse clicks on the checkbox area.
     * 
     * @param event The event to handle
     * @param model The item model
     * @param option Style options for the item
     * @param index Model index of the item
     * @return bool True if the event was handled, false otherwise
     */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    CategoryModel *m_categoryModel;  ///< Pointer to the category model for color info

    /**
     * @brief Calculate the rectangle for the checkbox
     * 
     * Helper method to determine the position and size of the checkbox.
     * 
     * @param option Style options for the item
     * @return QRect Rectangle defining the checkbox area
     */
    QRect checkboxRect(const QStyleOptionViewItem &option) const;
    
    /**
     * @brief Get color for a priority level
     * 
     * Maps priority levels to colors for visual indication.
     * 
     * @param priority Priority level (typically 1-5)
     * @return QColor Color representing the priority level
     */
    QColor priorityColor(int priority) const;
};
