/**
 * @file projectdelegate.h
 * @brief Definition of the ProjectDelegate class
 * 
 * This file defines the ProjectDelegate class which provides custom rendering
 * for Project items in list and combo box views.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include "../models/projectmodel.h"

/**
 * @class ProjectDelegate
 * @brief Custom delegate for rendering projects
 * 
 * The ProjectDelegate class provides custom rendering for projects in list views
 * and combo boxes, displaying the project color and name.
 */
class ProjectDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * 
     * Creates a new ProjectDelegate.
     * 
     * @param parent Optional parent object
     */
    explicit ProjectDelegate(QObject *parent = nullptr);
    
    /**
     * @brief Paint method
     * 
     * Renders a project item in a view.
     * 
     * @param painter Painter to use for rendering
     * @param option Style options for rendering
     * @param index Model index to render
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
    /**
     * @brief Size hint
     * 
     * Provides a hint for the size of a project item.
     * 
     * @param option Style options for rendering
     * @param index Model index to get size for
     * @return QSize Suggested size for the item
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
