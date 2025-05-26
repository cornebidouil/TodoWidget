/**
 * @file timereportsdialog.h
 * @brief Definition of the TimeReportsDialog class
 * 
 * This file defines the TimeReportsDialog class which provides a dialog
 * for viewing and exporting time tracking reports.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#pragma once

#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QMap>
#include <QDate>

/**
 * @class TimeReportsDialog
 * @brief Dialog for viewing and exporting time reports
 * 
 * The TimeReportsDialog class provides an interface for viewing time tracking data
 * with various filtering, grouping, and visualization options, as well as
 * the ability to export data for further analysis.
 */
class TimeReportsDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * 
     * Creates a new time reports dialog.
     * 
     * @param parent Parent widget
     */
    explicit TimeReportsDialog(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~TimeReportsDialog();

private slots:
    /**
     * @brief Handle date range change
     * 
     * Updates the report when the date range selection changes.
     * 
     * @param index Index of the selected date range in the combo box
     */
    void onDateRangeChanged(int index);
    
    /**
     * @brief Handle custom date change
     * 
     * Updates the report when the custom date range is changed.
     */
    void onCustomDateChanged();
    
    /**
     * @brief Handle grouping change
     * 
     * Updates the report when the grouping selection changes.
     * 
     * @param index Index of the selected grouping in the combo box
     */
    void onGroupingChanged(int index);
    
    /**
     * @brief Handle export button click
     * 
     * Exports the current report to a CSV file.
     */
    void onExportClicked();
    
    /**
     * @brief Handle refresh button click
     * 
     * Refreshes the report with current data.
     */
    void onRefreshClicked();

private:
    /**
     * @brief Set up the user interface
     * 
     * Creates and arranges all UI elements.
     */
    void setupUi();
    
    /**
     * @brief Set up signal-slot connections
     * 
     * Connects UI elements to their corresponding slots.
     */
    void setupConnections();
    
    /**
     * @brief Generate the report
     * 
     * Generates the report based on current settings.
     */
    void generateReport();
    
    /**
     * @brief Update the chart
     * 
     * Updates the visual chart based on the current report data.
     */
    void updateChart();
    
    /**
     * @brief Export the report to CSV
     * 
     * Exports the current report data to a CSV file.
     * 
     * @param filename Path to the output file
     * @return bool True if export was successful, false otherwise
     */
    bool exportToCsv(const QString& filename);
    
    /**
     * @brief Get the current date range
     * 
     * Calculates the start and end dates based on the current selection.
     * 
     * @param startDate Reference to store the start date
     * @param endDate Reference to store the end date
     */
    void getCurrentDateRange(QDate& startDate, QDate& endDate);
    
    /**
     * @brief Generate report by day
     * 
     * Generates a report grouped by day.
     * 
     * @param startDate Start date of the range
     * @param endDate End date of the range
     */
    void generateReportByDay(const QDate& startDate, const QDate& endDate);
    
    /**
     * @brief Generate report by project
     * 
     * Generates a report grouped by project.
     * 
     * @param startDate Start date of the range
     * @param endDate End date of the range
     */
    void generateReportByProject(const QDate& startDate, const QDate& endDate);

    // UI elements
    QComboBox* m_dateRangeComboBox;     ///< Combo box for selecting date range
    QDateEdit* m_customStartDateEdit;   ///< Date editor for custom start date
    QDateEdit* m_customEndDateEdit;     ///< Date editor for custom end date
    QComboBox* m_groupingComboBox;      ///< Combo box for selecting grouping
    QTableView* m_reportTableView;      ///< Table view for report data
    QFrame* m_chartFrame;               ///< Frame for displaying charts
    QPushButton* m_exportButton;        ///< Button for exporting data
    QPushButton* m_refreshButton;       ///< Button for refreshing the report
    QLabel* m_totalLabel;               ///< Label showing the total time
    
    // Data
    QStandardItemModel* m_reportModel;           ///< Model for report data
    QSortFilterProxyModel* m_proxyModel;         ///< Proxy model for sorting
    QMap<QString, QColor> m_colorMap;            ///< Map of labels to colors for charts
};