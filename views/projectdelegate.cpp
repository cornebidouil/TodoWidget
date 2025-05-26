/**
 * @file projectdelegate.cpp
 * @brief Implementation of the ProjectDelegate class
 * 
 * The ProjectDelegate class is responsible for custom rendering of Project items
 * in list and combo box views. It displays a color indicator alongside the project
 * name and shows the inactive status with italic font when applicable.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#include "projectdelegate.h"
#include <QPainter>
#include <QApplication>

/**
 * @brief Constructor for ProjectDelegate
 * 
 * @param parent The parent QObject
 */
ProjectDelegate::ProjectDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

/**
 * @brief Custom painting of project items
 * 
 * Overrides QStyledItemDelegate::paint() to provide custom rendering of Project items.
 * Each item shows a color square, the project name, and an "(Inactive)" indicator when needed.
 * 
 * @param painter The QPainter to use for drawing
 * @param option The style options for the item
 * @param index The model index of the item to draw
 */
void ProjectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QStyledItemDelegate::paint(painter, option, index);
    }
    
    // Draw selection background if selected
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
        painter->setPen(opt.palette.highlightedText().color());
    } else {
        painter->setPen(opt.palette.text().color());
    }
    
    // Get project data
    QString name = index.data(ProjectModel::NameRole).toString();
    QColor color = index.data(ProjectModel::ColorRole).value<QColor>();
    bool isActive = index.data(ProjectModel::IsActiveRole).toBool();
    
    // Draw color indicator
    QRect colorRect(opt.rect.left() + 5, opt.rect.top() + 5,
                    16, opt.rect.height() - 10);
    painter->fillRect(colorRect, color);
    painter->setPen(Qt::black);
    painter->drawRect(colorRect);
    
    // Draw project name with italics if inactive
    QFont font = opt.font;
    if (!isActive) {
        font.setItalic(true);
    }
    painter->setFont(font);
    
    QRect textRect = opt.rect.adjusted(30, 0, -5, 0);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, name);
    
    // Draw inactive indicator if needed
    if (!isActive) {
        QRect inactiveRect = textRect;
        inactiveRect.setLeft(textRect.left() + painter->fontMetrics().width(name) + 10);
        painter->drawText(inactiveRect, Qt::AlignVCenter | Qt::AlignLeft, "(Inactive)");
    }
}

/**
 * @brief Get the size hint for an item
 * 
 * Overrides QStyledItemDelegate::sizeHint() to provide a custom size hint for Project items.
 * 
 * @param option The style options for the item
 * @param index The model index of the item
 * @return The suggested size for the item
 */
QSize ProjectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 30);
}
