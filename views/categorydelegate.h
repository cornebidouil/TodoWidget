/**
 * @file categorydelegate.h
 * @brief Definition of the CategoryDelegate class
 * 
 * This file defines the CategoryDelegate class which customizes the rendering
 * of category items in list views and combo boxes. It provides visual representation
 * of category colors alongside their names.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QStyledItemDelegate>

/**
 * @class CategoryDelegate
 * @brief Custom delegate for rendering category items
 * 
 * The CategoryDelegate class extends QStyledItemDelegate to provide a
 * customized rendering of category items in views. It enhances the default
 * appearance by showing a color swatch representing the category color
 * alongside the category name.
 */
class CategoryDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * 
     * Creates a new CategoryDelegate.
     * 
     * @param parent Optional parent QObject
     */
    explicit CategoryDelegate(QObject *parent = nullptr);

    /**
     * @brief Custom painting implementation
     * 
     * Overrides the default painting to implement custom rendering of category items
     * with color swatches.
     * 
     * @param painter The painter to use for drawing
     * @param option Style options for the item
     * @param index Model index of the item to paint
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    /**
     * @brief Custom size hint implementation
     * 
     * Provides size information for category items.
     * 
     * @param option Style options for the item
     * @param index Model index of the item
     * @return QSize The recommended size for the item
     */
    QSize sizeHint(const QStyleOptionViewItem &option,
                  const QModelIndex &index) const override;
};
