/**
 * @file categorydelegate.cpp
 * @brief Implementation of the CategoryDelegate class
 * 
 * This file implements the CategoryDelegate class which provides custom
 * rendering for categories in list views. It displays each category with
 * its name and a color swatch representing the category's color.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "categorydelegate.h"
#include <QPainter>
#include "../models/categorymodel.h"

/**
 * @brief Constructor
 * 
 * Creates a new CategoryDelegate for custom category rendering.
 * 
 * @param parent Optional parent QObject
 */
CategoryDelegate::CategoryDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

/**
 * @brief Paint a category item
 * 
 * Renders a category item with a color swatch and the category name.
 * The color swatch is displayed as a small rectangle filled with the
 * category's color, followed by the category name as text.
 * 
 * @param painter The QPainter to use for drawing
 * @param option The style options for the item
 * @param index The model index of the category to paint
 */
void CategoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    // Get the color from the model
    QColor color = index.data(CategoryModel::ColorRole).value<QColor>();
    QString text = index.data(CategoryModel::NameRole).toString();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Draw the selection background if selected
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
        painter->setPen(opt.palette.highlightedText().color());
    } else {
        painter->setPen(opt.palette.text().color());
    }

    // Calculate rectangles
    QRect colorRect = opt.rect;
    colorRect.setWidth(20);
    colorRect.adjust(4, 4, -4, -4);

    QRect textRect = opt.rect;
    textRect.setLeft(colorRect.right() + 8);

    // Draw color rectangle
    painter->save();
    painter->setPen(Qt::black);
    painter->setBrush(color);
    painter->drawRect(colorRect);
    painter->restore();

    // Draw text
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
}

/**
 * @brief Get the size hint for the category item
 * 
 * Returns the suggested size for a category item, ensuring a minimum height
 * to make items easier to interact with and to provide consistent appearance.
 * 
 * @param option The style options for the item
 * @param index The model index of the category
 * @return QSize The suggested size for the item
 */
QSize CategoryDelegate::sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(qMax(size.height(), 30)); // Ensure minimum height
    return size;
}
