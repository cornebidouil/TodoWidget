/**
 * @file timereportsdialog.cpp
 * @brief Implementation of the TimeReportsDialog class
 * 
 * The TimeReportsDialog class provides a dialog for generating and viewing
 * time tracking reports. It allows filtering by date range, grouping by 
 * different criteria, and visualizing the data in charts. The dialog also
 * provides an export function to save reports as CSV files.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#include "timereportsdialog.h"
#include "../controllers/timetrackingcontroller.h"
#include "../controllers/projectcontroller.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QDebug>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

/**
 * @class ChartView
 * @brief Custom widget for chart visualization
 * 
 * This widget provides a canvas for drawing time tracking charts.
 * It can display data as either a pie chart or a bar chart.
 */
class ChartView : public QFrame {
public:
    /**
     * @brief Constructor for ChartView
     * 
     * @param parent The parent widget
     */
    explicit ChartView(QWidget* parent = nullptr) : QFrame(parent), m_totalValue(0) {
        setMinimumSize(300, 200);
        setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        setBackgroundRole(QPalette::Base);
        setAutoFillBackground(true);
    }
    
    /**
     * @brief Set the data to display in the chart
     * 
     * @param data A map of labels to values
     * @param colors A map of labels to colors
     * @param total The total value (sum of all values in data)
     */
    void setData(const QMap<QString, int>& data, const QMap<QString, QColor>& colors, int total) {
        m_data = data;
        m_colors = colors;
        m_totalValue = total;
        update();
    }
    
    /**
     * @brief Set the chart type
     * 
     * @param type The chart type, either "pie" or "bar"
     */
    void setChartType(const QString& type) {
        m_chartType = type;
        update();
    }
    
protected:
    /**
     * @brief Paint event handler
     * 
     * Draws either a pie chart or bar chart based on the current type.
     * 
     * @param event The paint event
     */
    void paintEvent(QPaintEvent* event) override {
        QFrame::paintEvent(event);
        
        if (m_data.isEmpty() || m_totalValue <= 0) {
            QPainter painter(this);
            painter.drawText(rect(), Qt::AlignCenter, "No data to display");
            return;
        }
        
        if (m_chartType == "pie") {
            drawPieChart();
        } else {
            drawBarChart();
        }
    }
    
private:
    /**
     * @brief Draw a pie chart
     * 
     * Draws a pie chart with segments for each data item.
     */
    void drawPieChart() {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Calculate chart size
        int margin = 20;
        QRect chartRect = rect().adjusted(margin, margin, -margin, -margin);
        int size = qMin(chartRect.width(), chartRect.height());
        QRect pieRect = QRect(chartRect.center().x() - size/2, chartRect.center().y() - size/2, size, size);
        
        // Draw the pie segments
        qreal startAngle = 0;
        int i = 0;
        
        QStringList legend;
        
        for (auto it = m_data.begin(); it != m_data.end(); ++it) {
            QString label = it.key();
            int value = it.value();
            QColor color = m_colors.value(label, QColor::fromHsv(i * 360 / m_data.size(), 200, 230));
            
            qreal sweepAngle = 360.0 * value / m_totalValue;
            
            painter.setBrush(color);
            painter.setPen(Qt::white);
            painter.drawPie(pieRect, startAngle * 16, sweepAngle * 16);
            
            // Calculate percent for legend
            double percent = (double)value / m_totalValue * 100.0;
            legend.append(QString("%1: %2 (%3%)").arg(label).arg(TimeTrackingController::formatDuration(value)).arg(percent, 0, 'f', 1));
            
            startAngle += sweepAngle;
            i++;
        }
        
        // Draw legend
        painter.setPen(Qt::black);
        int legendY = pieRect.bottom() + 20;
        for (const QString& text : legend) {
            painter.drawText(QRect(margin, legendY, width() - 2*margin, 20), text);
            legendY += 20;
        }
    }
    
    /**
     * @brief Draw a bar chart
     * 
     * Draws a bar chart with bars for each data item.
     */
    void drawBarChart() {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Calculate chart size
        int margin = 20;
        int legendHeight = m_data.size() * 20 + 20;  // Space for legend
        QRect chartRect = rect().adjusted(margin, margin, -margin, -margin - legendHeight);
        
        if (chartRect.height() < 50 || chartRect.width() < 50) {
            painter.drawText(rect(), Qt::AlignCenter, "Chart area too small");
            return;
        }
        
        // Draw axes
        painter.setPen(Qt::black);
        painter.drawLine(chartRect.bottomLeft(), chartRect.bottomRight());  // X axis
        painter.drawLine(chartRect.bottomLeft(), chartRect.topLeft());      // Y axis
        
        // Calculate bar width and spacing
        int count = m_data.size();
        int barWidth = chartRect.width() / (count * 2);
        int maxBarHeight = chartRect.height() - 10;
        
        // Find maximum value for scaling
        int maxValue = 0;
        for (auto it = m_data.begin(); it != m_data.end(); ++it) {
            maxValue = qMax(maxValue, it.value());
        }
        
        // Draw bars
        int i = 0;
        QStringList legend;
        
        for (auto it = m_data.begin(); it != m_data.end(); ++it) {
            QString label = it.key();
            int value = it.value();
            QColor color = m_colors.value(label, QColor::fromHsv(i * 360 / count, 200, 230));
            
            int barHeight = maxValue > 0 ? value * maxBarHeight / maxValue : 0;
            int barX = chartRect.left() + i * barWidth * 2 + barWidth/2;
            int barY = chartRect.bottom() - barHeight;
            
            QRect barRect(barX, barY, barWidth, barHeight);
            
            painter.setBrush(color);
            painter.setPen(Qt::black);
            painter.drawRect(barRect);
            
            // Draw value on top
            QString valueText = TimeTrackingController::formatDuration(value, "h:mm");
            painter.drawText(QRect(barX - barWidth, barY - 20, barWidth*3, 20), 
                           Qt::AlignCenter, valueText);
            
            // Prepare legend entry
            double percent = (double)value / m_totalValue * 100.0;
            legend.append(QString("%1: %2 (%3%)").arg(label).arg(TimeTrackingController::formatDuration(value)).arg(percent, 0, 'f', 1));
            
            i++;
        }
        
        // Draw legend
        int legendY = chartRect.bottom() + 20;
        for (const QString& text : legend) {
            painter.drawText(QRect(margin, legendY, width() - 2*margin, 20), text);
            legendY += 20;
        }
    }
    
    QMap<QString, int> m_data;           ///< Chart data (label -> value)
    QMap<QString, QColor> m_colors;      ///< Colors for chart elements
    int m_totalValue;                    ///< Total value for percentage calculation
    QString m_chartType = "bar";         ///< Chart type: "bar" or "pie"
};

/**
 * @brief Constructor for TimeReportsDialog
 * 
 * Initializes the dialog and sets up the UI elements and connections.
 * 
 * @param parent The parent widget
 */
TimeReportsDialog::TimeReportsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Time Reports");
    
    // Initialize models
    m_reportModel = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_reportModel);
    
    setupUi();
    setupConnections();
    
    // Set default date range to "This Week"
    m_dateRangeComboBox->setCurrentIndex(2);  // This Week
    onDateRangeChanged(2);
}

/**
 * @brief Destructor for TimeReportsDialog
 */
TimeReportsDialog::~TimeReportsDialog()
{
}

/**
 * @brief Set up the user interface
 * 
 * Creates and arranges all UI elements for the dialog.
 */
void TimeReportsDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Filter group
    QGroupBox* filterGroup = new QGroupBox("Report Options", this);
    QVBoxLayout* filterLayout = new QVBoxLayout(filterGroup);
    
    // Date range selection
    QHBoxLayout* dateRangeLayout = new QHBoxLayout();
    QLabel* dateRangeLabel = new QLabel("Date Range:", this);
    m_dateRangeComboBox = new QComboBox(this);
    m_dateRangeComboBox->addItems(QStringList()
                                << "Today"
                                << "Yesterday"
                                << "This Week"
                                << "Last Week"
                                << "This Month"
                                << "Last Month"
                                << "Custom Range");
    
    dateRangeLayout->addWidget(dateRangeLabel);
    dateRangeLayout->addWidget(m_dateRangeComboBox);
    filterLayout->addLayout(dateRangeLayout);
    
    // Custom date range
    QHBoxLayout* customDateLayout = new QHBoxLayout();
    QLabel* fromLabel = new QLabel("From:", this);
    m_customStartDateEdit = new QDateEdit(this);
    m_customStartDateEdit->setCalendarPopup(true);
    m_customStartDateEdit->setDate(QDate::currentDate().addDays(-7));
    
    QLabel* toLabel = new QLabel("To:", this);
    m_customEndDateEdit = new QDateEdit(this);
    m_customEndDateEdit->setCalendarPopup(true);
    m_customEndDateEdit->setDate(QDate::currentDate());
    
    customDateLayout->addWidget(fromLabel);
    customDateLayout->addWidget(m_customStartDateEdit);
    customDateLayout->addWidget(toLabel);
    customDateLayout->addWidget(m_customEndDateEdit);
    filterLayout->addLayout(customDateLayout);
    
    // Grouping
    QHBoxLayout* groupingLayout = new QHBoxLayout();
    QLabel* groupingLabel = new QLabel("Group By:", this);
    m_groupingComboBox = new QComboBox(this);
    m_groupingComboBox->addItems(QStringList() << "Day" << "Project");
    
    groupingLayout->addWidget(groupingLabel);
    groupingLayout->addWidget(m_groupingComboBox);
    groupingLayout->addStretch();
    
    m_refreshButton = new QPushButton("Refresh", this);
    groupingLayout->addWidget(m_refreshButton);
    
    filterLayout->addLayout(groupingLayout);
    
    mainLayout->addWidget(filterGroup);
    
    // Results group
    QGroupBox* resultsGroup = new QGroupBox("Results", this);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsGroup);
    
    // Table view for results
    m_reportTableView = new QTableView(this);
    m_reportTableView->setModel(m_proxyModel);
    m_reportTableView->setSortingEnabled(true);
    m_reportTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_reportTableView->verticalHeader()->setVisible(false);
    m_reportTableView->setAlternatingRowColors(true);
    m_reportTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsLayout->addWidget(m_reportTableView);
    
    // Total time
    m_totalLabel = new QLabel("Total: 0:00", this);
    QFont totalFont = m_totalLabel->font();
    totalFont.setBold(true);
    m_totalLabel->setFont(totalFont);
    m_totalLabel->setAlignment(Qt::AlignRight);
    resultsLayout->addWidget(m_totalLabel);
    
    mainLayout->addWidget(resultsGroup);
    
    // Chart group
    QGroupBox* chartGroup = new QGroupBox("Visualization", this);
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    
    m_chartFrame = new ChartView(this);
    chartLayout->addWidget(m_chartFrame);
    
    mainLayout->addWidget(chartGroup);
    
    // Export button
    m_exportButton = new QPushButton("Export to CSV", this);
    mainLayout->addWidget(m_exportButton);
    
    // Set layout sizes
    mainLayout->setStretchFactor(filterGroup, 0);
    mainLayout->setStretchFactor(resultsGroup, 1);
    mainLayout->setStretchFactor(chartGroup, 1);
    
    setMinimumSize(600, 600);
}

/**
 * @brief Set up signal/slot connections
 * 
 * Connects UI elements to their respective slots.
 */
void TimeReportsDialog::setupConnections()
{
    connect(m_dateRangeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &TimeReportsDialog::onDateRangeChanged);
    
    connect(m_customStartDateEdit, &QDateEdit::dateChanged, 
            this, &TimeReportsDialog::onCustomDateChanged);
    
    connect(m_customEndDateEdit, &QDateEdit::dateChanged, 
            this, &TimeReportsDialog::onCustomDateChanged);
    
    connect(m_groupingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &TimeReportsDialog::onGroupingChanged);
    
    connect(m_exportButton, &QPushButton::clicked, 
            this, &TimeReportsDialog::onExportClicked);
    
    connect(m_refreshButton, &QPushButton::clicked, 
            this, &TimeReportsDialog::onRefreshClicked);
}

/**
 * @brief Handle date range combo box change
 * 
 * Enables/disables custom date inputs based on selection and updates the report.
 * 
 * @param index The index of the selected date range option
 */
void TimeReportsDialog::onDateRangeChanged(int index)
{
    // Enable/disable custom date range based on selection
    bool isCustomRange = (index == 6);  // "Custom Range"
    m_customStartDateEdit->setEnabled(isCustomRange);
    m_customEndDateEdit->setEnabled(isCustomRange);
    
    // Update the report
    generateReport();
}

/**
 * @brief Handle custom date changes
 * 
 * Updates the report when custom date range is changed.
 */
void TimeReportsDialog::onCustomDateChanged()
{
    // Only update if custom range is selected
    if (m_dateRangeComboBox->currentIndex() == 6) {
        generateReport();
    }
}

/**
 * @brief Handle grouping combo box change
 * 
 * Updates the report when the grouping option is changed.
 * 
 * @param index The index of the selected grouping option
 */
void TimeReportsDialog::onGroupingChanged(int index)
{
    generateReport();
}

/**
 * @brief Handle refresh button click
 * 
 * Regenerates the report with current filter settings.
 */
void TimeReportsDialog::onRefreshClicked()
{
    generateReport();
}

/**
 * @brief Handle export button click
 * 
 * Opens a file dialog and exports the report to a CSV file.
 */
void TimeReportsDialog::onExportClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export Report", 
                                                  QDir::homePath(), 
                                                  "CSV Files (*.csv)");
    
    if (!filename.isEmpty()) {
        if (!filename.endsWith(".csv", Qt::CaseInsensitive)) {
            filename += ".csv";
        }
        
        if (exportToCsv(filename)) {
            QMessageBox::information(this, "Export Successful", 
                                   "The report has been exported to " + filename);
        } else {
            QMessageBox::warning(this, "Export Failed", 
                               "Failed to export the report to " + filename);
        }
    }
}

/**
 * @brief Get the current date range based on selection
 * 
 * Calculates start and end dates based on the selected date range option.
 * 
 * @param startDate Output parameter for the start date
 * @param endDate Output parameter for the end date
 */
void TimeReportsDialog::getCurrentDateRange(QDate& startDate, QDate& endDate)
{
    QDate today = QDate::currentDate();
    
    switch (m_dateRangeComboBox->currentIndex()) {
        case 0:  // Today
            startDate = today;
            endDate = today;
            break;
            
        case 1:  // Yesterday
            startDate = today.addDays(-1);
            endDate = today.addDays(-1);
            break;
            
        case 2:  // This Week
            startDate = today.addDays(-(today.dayOfWeek() - 1));
            endDate = startDate.addDays(6);
            break;
            
        case 3:  // Last Week
            startDate = today.addDays(-(today.dayOfWeek() + 6));
            endDate = startDate.addDays(6);
            break;
            
        case 4:  // This Month
            startDate = QDate(today.year(), today.month(), 1);
            endDate = QDate(today.year(), today.month(), today.daysInMonth());
            break;
            
        case 5:  // Last Month
            if (today.month() == 1) {
                startDate = QDate(today.year() - 1, 12, 1);
                endDate = QDate(today.year() - 1, 12, 31);
            } else {
                startDate = QDate(today.year(), today.month() - 1, 1);
                endDate = QDate(today.year(), today.month() - 1, QDate(today.year(), today.month() - 1, 1).daysInMonth());
            }
            break;
            
        case 6:  // Custom Range
            startDate = m_customStartDateEdit->date();
            endDate = m_customEndDateEdit->date();
            break;
            
        default:
            startDate = today;
            endDate = today;
            break;
    }
}

/**
 * @brief Generate a time report with current filter settings
 * 
 * Retrieves data based on current filter settings and updates the model and chart.
 */
void TimeReportsDialog::generateReport()
{
    QDate startDate, endDate;
    getCurrentDateRange(startDate, endDate);
    
    m_reportModel->clear();
    
    // Generate report based on grouping
    switch (m_groupingComboBox->currentIndex()) {
        case 0:  // Day
            generateReportByDay(startDate, endDate);
            break;
            
        case 1:  // Project
            generateReportByProject(startDate, endDate);
            break;
    }
    
    m_reportTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_reportTableView->resizeColumnsToContents();
    m_reportTableView->resizeRowsToContents();
    
    updateChart();
}

/**
 * @brief Generate a report grouped by day
 * 
 * Creates a report showing time spent on each day in the date range.
 * 
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 */
void TimeReportsDialog::generateReportByDay(const QDate& startDate, const QDate& endDate)
{
    m_reportModel->setColumnCount(3);
    m_reportModel->setHorizontalHeaderLabels(QStringList() << "Date" << "Hours" << "Minutes");
    
    QMap<QDate, int> dailyTotals = TimeTrackingController::instance().getTimeByDay(startDate, endDate);
    
    int totalSeconds = 0;
    int row = 0;
    
    for (QDate date = startDate; date <= endDate; date = date.addDays(1)) {
        int seconds = dailyTotals.value(date, 0);
        totalSeconds += seconds;
        
        if (seconds > 0 || true) {  // Show all days, even with 0 time
            m_reportModel->insertRow(row);
            
            // Date
            m_reportModel->setItem(row, 0, new QStandardItem(date.toString("yyyy-MM-dd")));
            
            // Hours
            int hours = seconds / 3600;
            m_reportModel->setItem(row, 1, new QStandardItem(QString::number(hours)));
            
            // Minutes
            int minutes = (seconds % 3600) / 60;
            m_reportModel->setItem(row, 2, new QStandardItem(QString::number(minutes)));
            
            row++;
        }
    }
    
    // Update total
    QString totalFormatted = TimeTrackingController::formatDuration(totalSeconds);
    m_totalLabel->setText("Total: " + totalFormatted);
}

/**
 * @brief Generate a report grouped by project
 * 
 * Creates a report showing time spent on each project in the date range.
 * 
 * @param startDate The start date of the range
 * @param endDate The end date of the range
 */
void TimeReportsDialog::generateReportByProject(const QDate& startDate, const QDate& endDate)
{
    m_reportModel->setColumnCount(3);
    m_reportModel->setHorizontalHeaderLabels(QStringList() << "Project" << "Hours" << "Minutes");
    
    QMap<QString, int> projectTotals = TimeTrackingController::instance().getTimeByProject(startDate, endDate);
    
    int totalSeconds = 0;
    int row = 0;
    
    // Clear color map
    m_colorMap.clear();
    
    for (auto it = projectTotals.begin(); it != projectTotals.end(); ++it) {
        QString projectId = it.key();
        int seconds = it.value();
        
        if (seconds > 0) {
            totalSeconds += seconds;
            
            Project project = ProjectController::instance().getProject(projectId);
            QString projectName = project.id().isEmpty() ? "Unknown Project" : project.name();
            
            m_reportModel->insertRow(row);
            
            // Project name
            m_reportModel->setItem(row, 0, new QStandardItem(projectName));
            
            // Hours
            int hours = seconds / 3600;
            m_reportModel->setItem(row, 1, new QStandardItem(QString::number(hours)));
            
            // Minutes
            int minutes = (seconds % 3600) / 60;
            m_reportModel->setItem(row, 2, new QStandardItem(QString::number(minutes)));
            
            // Store color for chart
            m_colorMap[projectName] = project.id().isEmpty() ? QColor(200, 200, 200) : project.color();
            
            row++;
        }
    }
    
    // Update total
    QString totalFormatted = TimeTrackingController::formatDuration(totalSeconds);
    m_totalLabel->setText("Total: " + totalFormatted);
}

/**
 * @brief Update the chart with current report data
 * 
 * Extracts data from the model and updates the chart visualization.
 */
void TimeReportsDialog::updateChart()
{
    QMap<QString, int> chartData;
    int totalValue = 0;
    
    // Extract data from model
    for (int row = 0; row < m_reportModel->rowCount(); ++row) {
        QString label = m_reportModel->item(row, 0)->text();
        int hours = m_reportModel->item(row, 1)->text().toInt();
        int minutes = m_reportModel->item(row, 2)->text().toInt();
        
        int seconds = hours * 3600 + minutes * 60;
        chartData[label] = seconds;
        totalValue += seconds;
    }
    
    // Choose chart type based on data size
    QString chartType = chartData.size() <= 5 ? "pie" : "bar";
    
    // Draw chart
    // Use static_cast instead of qobject_cast since ChartView doesn't have Q_OBJECT macro
    ChartView* chartView = static_cast<ChartView*>(m_chartFrame);
    if (chartView) {
        chartView->setData(chartData, m_colorMap, totalValue);
        chartView->setChartType(chartType);
    }
}

/**
 * @brief Export the current report to a CSV file
 * 
 * Writes the report data to a CSV file, including headers and total.
 * 
 * @param filename The path to the CSV file to create
 * @return true if the export was successful, false otherwise
 */
bool TimeReportsDialog::exportToCsv(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    QStringList headers;
    for (int i = 0; i < m_reportModel->columnCount(); ++i) {
        headers << m_reportModel->horizontalHeaderItem(i)->text();
    }
    out << headers.join(",") << "\n";
    
    // Write data
    for (int row = 0; row < m_reportModel->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < m_reportModel->columnCount(); ++col) {
            rowData << "\"" + m_reportModel->item(row, col)->text() + "\"";
        }
        out << rowData.join(",") << "\n";
    }
    
    // Write total
    out << "\n\"Total\"," << "\"" << m_totalLabel->text().replace("Total: ", "") << "\"\n";
    
    file.close();
    return true;
}