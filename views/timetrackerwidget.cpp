/**
 * @file timetrackerwidget.cpp
 * @brief Implementation of the TimeTrackerWidget class
 * 
 * The TimeTrackerWidget class provides the main user interface for time tracking.
 * It allows users to select projects, start and stop time tracking, add manual time
 * entries, and view recent time entries. It also shows summary information like today's
 * total time, weekly total, and most tracked project.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#include "timetrackerwidget.h"
#include "timeentrydialog.h"
#include "timereportsdialog.h"
#include "projecteditor.h"
#include "../controllers/projectcontroller.h"
#include <QMessageBox>
#include <QMenu>
#include <QHeaderView>
#include <QDateTime>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QDebug>

/**
 * @class TimeEntryDelegate
 * @brief Custom delegate for rendering time entries
 * 
 * This delegate provides a custom rendering for time entries in the list view,
 * showing the project name, duration, time, and project color.
 */
class TimeEntryDelegate : public QStyledItemDelegate {
public:
    /**
     * @brief Constructor for TimeEntryDelegate
     * 
     * @param parent The parent QObject
     */
    explicit TimeEntryDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    
    /**
     * @brief Custom painting of time entry items
     * 
     * Overrides QStyledItemDelegate::paint() to provide custom rendering of TimeEntry items.
     * Each item shows a color bar matching the project color, the project name, and time information.
     * 
     * @param painter The QPainter to use for drawing
     * @param option The style options for the item
     * @param index The model index of the item to draw
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (!index.isValid()) {
            return QStyledItemDelegate::paint(painter, option, index);
        }
        
        // Get data from the model
        QString projectId = index.data(TimeEntryModel::ProjectIdRole).toString();
        QDateTime startTime = index.data(TimeEntryModel::StartTimeRole).toDateTime();
        int duration = index.data(TimeEntryModel::DurationRole).toInt();
        bool isRunning = index.data(TimeEntryModel::IsRunningRole).toBool();
        
        // Get project information
        Project project = ProjectController::instance().getProject(projectId);
        QString projectName = project.name();
        QColor projectColor = project.color();
        
        // Prepare to draw
        painter->save();
        
        // Draw selection background if selected
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(option.palette.highlightedText().color());
        } else {
            painter->setPen(option.palette.text().color());
        }
        
        // Draw project color bar
        QRect colorBar(option.rect.left(), option.rect.top(), 5, option.rect.height());
        painter->fillRect(colorBar, projectColor);
        
        // Calculate item layout
        QRect contentRect = option.rect;
        contentRect.setLeft(contentRect.left() + 10);
        
        // Draw project name
        QFont boldFont = option.font;
        boldFont.setBold(true);
        painter->setFont(boldFont);
        painter->drawText(
            QRect(contentRect.left(), contentRect.top(), contentRect.width() - 10, 20),
            Qt::AlignLeft | Qt::AlignVCenter,
            projectName
        );
        
        // Draw time information
        painter->setFont(option.font);
        QString timeText = startTime.toString("hh:mm") + (isRunning ? " → Running" : " → " + 
                          TimeTrackingController::formatDuration(duration, "h:mm:ss"));
        painter->drawText(
            QRect(contentRect.left(), contentRect.top() + 20, contentRect.width() - 10, 20),
            Qt::AlignLeft | Qt::AlignVCenter,
            timeText
        );
        
        // Draw date
        QString dateText = startTime.toString("yyyy-MM-dd");
        QFontMetrics fm(option.font);
        int dateWidth = fm.width(dateText);
        painter->drawText(
            QRect(contentRect.right() - dateWidth - 10, contentRect.top(), dateWidth, 20),
            Qt::AlignRight | Qt::AlignVCenter,
            dateText
        );
        
        painter->restore();
    }
    
    /**
     * @brief Get the size hint for an item
     * 
     * Overrides QStyledItemDelegate::sizeHint() to provide a custom size hint for TimeEntry items.
     * 
     * @param option The style options for the item
     * @param index The model index of the item
     * @return The suggested size for the item
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        return QSize(option.rect.width(), 50);
    }
};

/**
 * @brief Constructor for TimeTrackerWidget
 * 
 * Initializes the widget and sets up the UI elements and connections.
 * Checks if there's already a running timer and updates the UI accordingly.
 * 
 * @param parent The parent widget
 */
TimeTrackerWidget::TimeTrackerWidget(QWidget* parent)
    : QWidget(parent)
    , m_controller(TimeTrackingController::instance())
    , m_isTracking(false)
    , m_timeEntryDialog(nullptr)
    , m_reportsDialog(nullptr)
{
    setupUi();
    setupConnections();
    updateSummary();
    
    // We'll call updateProjectComboBox once projects are loaded, but here's a placeholder call
    updateProjectComboBox();
    
    // Check if there's a running timer
    if (m_controller.isTimerRunning()) {
        m_isTracking = true;
        m_startStopButton->setText("Stop");
        
        // Use setCurrentIndex with the data role to properly select the project
        QString currentProjectId = m_controller.getCurrentProjectId();
        int index = m_projectComboBox->findData(currentProjectId);
        if (index >= 0) {
            m_projectComboBox->setCurrentIndex(index);
        }
        
        updateTimerDisplay(m_controller.getCurrentElapsed());
    }
}

/**
 * @brief Destructor for TimeTrackerWidget
 * 
 * Cleans up any dynamically allocated dialogs.
 */
TimeTrackerWidget::~TimeTrackerWidget()
{
    if (m_timeEntryDialog) {
        delete m_timeEntryDialog;
    }
    
    if (m_reportsDialog) {
        delete m_reportsDialog;
    }
}

/**
 * @brief Set up the user interface
 * 
 * Creates and arranges all UI elements for the widget.
 */
void TimeTrackerWidget::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Timer control group
    QGroupBox* timerGroup = new QGroupBox("Time Tracker", this);
    QVBoxLayout* timerLayout = new QVBoxLayout(timerGroup);
    
    // Project selection
    QHBoxLayout* projectLayout = new QHBoxLayout();
    QLabel* projectLabel = new QLabel("Project:", this);
    m_projectComboBox = new QComboBox(this);
    //m_projectComboBox->setMinimumWidth(200);
    projectLayout->addWidget(projectLabel);
    projectLayout->addWidget(m_projectComboBox, 1);
    timerLayout->addLayout(projectLayout);
    
    // Timer controls
    QHBoxLayout* controlLayout = new QHBoxLayout();
    m_timerLabel = new QLabel("00:00:00", this);
    QFont timerFont = m_timerLabel->font();
    timerFont.setPointSize(timerFont.pointSize() * 2);
    timerFont.setBold(true);
    m_timerLabel->setFont(timerFont);
    m_timerLabel->setAlignment(Qt::AlignCenter);
    m_startStopButton = new QPushButton("Start", this);
    m_startStopButton->setMinimumWidth(80);
    
    controlLayout->addWidget(m_timerLabel, 1);
    controlLayout->addWidget(m_startStopButton);
    timerLayout->addLayout(controlLayout);
    
    // Add button for manual entry
    m_addManualButton = new QPushButton("Add Manual Entry", this);
    timerLayout->addWidget(m_addManualButton);
    
    mainLayout->addWidget(timerGroup);
    
    // Recent entries group
    QGroupBox* entriesGroup = new QGroupBox("Recent Time Entries", this);
    QVBoxLayout* entriesLayout = new QVBoxLayout(entriesGroup);
    
    m_recentEntriesView = new QListView(this);
    m_recentEntriesView->setModel(m_controller.timeEntryModel());
    m_recentEntriesView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_recentEntriesView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_recentEntriesView->setItemDelegate(new TimeEntryDelegate(this));
    m_recentEntriesView->setAlternatingRowColors(true);
    m_recentEntriesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    entriesLayout->addWidget(m_recentEntriesView);
    
    // Reports button
    m_reportsButton = new QPushButton("View Reports", this);
    entriesLayout->addWidget(m_reportsButton);
    
    mainLayout->addWidget(entriesGroup);
    
    // Summary group
    QGroupBox* summaryGroup = new QGroupBox("Summary", this);
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryGroup);
    
    m_todayTotalLabel = new QLabel("Today: 0:00", this);
    m_weekTotalLabel = new QLabel("This Week: 0:00", this);
    m_mostTrackedLabel = new QLabel("Most Tracked: None", this);
    
    summaryLayout->addWidget(m_todayTotalLabel);
    summaryLayout->addWidget(m_weekTotalLabel);
    summaryLayout->addWidget(m_mostTrackedLabel);
    
    mainLayout->addWidget(summaryGroup);
    
    // Set layout stretching
    mainLayout->setStretchFactor(timerGroup, 0);
    mainLayout->setStretchFactor(entriesGroup, 1);
    mainLayout->setStretchFactor(summaryGroup, 0);
}

/**
 * @brief Set up signal/slot connections
 * 
 * Connects UI elements to their respective slots and controller signals to their handlers.
 */
void TimeTrackerWidget::setupConnections()
{
    // Timer controls
    connect(m_startStopButton, &QPushButton::clicked, this, &TimeTrackerWidget::onStartStopClicked);
    connect(m_projectComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TimeTrackerWidget::onProjectSelectionChanged);
    connect(m_addManualButton, &QPushButton::clicked, this, &TimeTrackerWidget::onAddManualEntryClicked);
    
    // Entry list
    connect(m_recentEntriesView, &QListView::doubleClicked, this, &TimeTrackerWidget::onEntryDoubleClicked);
    connect(m_recentEntriesView, &QListView::customContextMenuRequested, this, &TimeTrackerWidget::onContextMenuRequested);
    
    // Reports
    connect(m_reportsButton, &QPushButton::clicked, this, &TimeTrackerWidget::onReportsClicked);
    
    // Controller signals
    connect(&m_controller, &TimeTrackingController::timerTick, this, &TimeTrackerWidget::onTimerTick);
    connect(&m_controller, &TimeTrackingController::timerStarted, this, &TimeTrackerWidget::onTimerStarted);
    connect(&m_controller, &TimeTrackingController::timerStopped, this, &TimeTrackerWidget::onTimerStopped);
    connect(&m_controller, &TimeTrackingController::timeEntryAdded, this, &TimeTrackerWidget::onTimeEntryAdded);
    connect(&m_controller, &TimeTrackingController::timeEntryUpdated, this, &TimeTrackerWidget::onTimeEntryUpdated);
    connect(&m_controller, &TimeTrackingController::timeEntryDeleted, this, &TimeTrackerWidget::onTimeEntryDeleted);
    
    // ProjectController signals for updating project list
    connect(&ProjectController::instance(), &ProjectController::projectAdded, this, &TimeTrackerWidget::updateProjectComboBox);
    connect(&ProjectController::instance(), &ProjectController::projectUpdated, this, &TimeTrackerWidget::updateProjectComboBox);
    connect(&ProjectController::instance(), &ProjectController::projectDeleted, this, &TimeTrackerWidget::updateProjectComboBox);
    connect(&ProjectController::instance(), &ProjectController::projectsChanged, this, &TimeTrackerWidget::updateProjectComboBox);
}

/**
 * @brief Handle Start/Stop button click
 * 
 * Starts or stops the timer based on the current tracking state.
 */
void TimeTrackerWidget::onStartStopClicked()
{
    if (m_isTracking) {
        // Stop tracking
        m_controller.stopTimer();
    } else {
        // Start tracking
        QString projectId = m_projectComboBox->currentData().toString();
        if (projectId.isEmpty()) {
            QMessageBox::warning(this, "No Project Selected", "Please select a project to track time for.");
            return;
        }
        
        m_controller.startTimer(projectId);
    }
}

/**
 * @brief Handle project selection changes
 * 
 * If tracking is active, asks the user if they want to switch projects.
 * 
 * @param index The index of the newly selected project in the combo box
 */
void TimeTrackerWidget::onProjectSelectionChanged(int index)
{
    // If tracking is active, ask if user wants to switch projects
    if (m_isTracking && index >= 0) {
        QString newProjectId = m_projectComboBox->currentData().toString();
        if (newProjectId != m_controller.getCurrentProjectId()) {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this, "Switch Project?", 
                "Do you want to stop tracking the current project and start tracking the selected project?",
                QMessageBox::Yes | QMessageBox::No
            );
            
            if (reply == QMessageBox::Yes) {
                m_controller.stopTimer();
                m_controller.startTimer(newProjectId);
            } else {
                // Revert selection
                int oldIndex = m_projectComboBox->findData(m_controller.getCurrentProjectId());
                if (oldIndex >= 0) {
                    QSignalBlocker blocker(m_projectComboBox);
                    m_projectComboBox->setCurrentIndex(oldIndex);
                }
            }
        }
    }
}

/**
 * @brief Handle timer tick signal from controller
 * 
 * Updates the timer display with the current elapsed time.
 * 
 * @param elapsed The elapsed time in seconds
 */
void TimeTrackerWidget::onTimerTick(int elapsed)
{
    updateTimerDisplay(elapsed);
}

/**
 * @brief Update the timer display
 * 
 * Updates the timer label with the formatted duration.
 * 
 * @param seconds The duration in seconds
 */
void TimeTrackerWidget::updateTimerDisplay(int seconds)
{
    m_timerLabel->setText(TimeTrackingController::formatDuration(seconds));
}

/**
 * @brief Handle Add Manual Entry button click
 * 
 * Opens a TimeEntryDialog for adding a manual time entry.
 */
void TimeTrackerWidget::onAddManualEntryClicked()
{
    if (!m_timeEntryDialog) {
        m_timeEntryDialog = new TimeEntryDialog(this);
    }
    
    if (m_timeEntryDialog->exec() == QDialog::Accepted) {
        TimeEntry entry = m_timeEntryDialog->getTimeEntry();
        m_controller.addTimeEntry(entry);
    }
}

/**
 * @brief Handle Reports button click
 * 
 * Opens the TimeReportsDialog for viewing time reports.
 */
void TimeTrackerWidget::onReportsClicked()
{
    if (!m_reportsDialog) {
        m_reportsDialog = new TimeReportsDialog(this);
    }
    
    m_reportsDialog->exec();
}

/**
 * @brief Handle Edit action from context menu
 * 
 * Opens a TimeEntryDialog for editing the selected time entry.
 */
void TimeTrackerWidget::onEditEntryClicked()
{
    QModelIndex currentIndex = m_recentEntriesView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }
    
    QString entryId = currentIndex.data(TimeEntryModel::IdRole).toString();
    TimeEntry entry = m_controller.timeEntryModel()->getTimeEntry(entryId);
    
    if (!m_timeEntryDialog) {
        m_timeEntryDialog = new TimeEntryDialog(this);
    }
    
    m_timeEntryDialog->setTimeEntry(entry);
    
    if (m_timeEntryDialog->exec() == QDialog::Accepted) {
        TimeEntry updatedEntry = m_timeEntryDialog->getTimeEntry();
        m_controller.updateTimeEntry(updatedEntry);
    }
}

/**
 * @brief Handle Delete action from context menu
 * 
 * Confirms with the user and then deletes the selected time entry.
 */
void TimeTrackerWidget::onDeleteEntryClicked()
{
    QModelIndex currentIndex = m_recentEntriesView->currentIndex();
    if (!currentIndex.isValid()) {
        return;
    }
    
    QString entryId = currentIndex.data(TimeEntryModel::IdRole).toString();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Deletion", 
        "Are you sure you want to delete this time entry?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_controller.deleteTimeEntry(entryId);
    }
}

/**
 * @brief Handle double-click on a time entry
 * 
 * Opens a TimeEntryDialog for editing the clicked time entry.
 * 
 * @param index The model index of the clicked time entry
 */
void TimeTrackerWidget::onEntryDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }
    
    QString entryId = index.data(TimeEntryModel::IdRole).toString();
    TimeEntry entry = m_controller.timeEntryModel()->getTimeEntry(entryId);
    
    if (!m_timeEntryDialog) {
        m_timeEntryDialog = new TimeEntryDialog(this);
    }
    
    m_timeEntryDialog->setTimeEntry(entry);
    
    if (m_timeEntryDialog->exec() == QDialog::Accepted) {
        TimeEntry updatedEntry = m_timeEntryDialog->getTimeEntry();
        m_controller.updateTimeEntry(updatedEntry);
    }
}

/**
 * @brief Handle context menu request on time entries view
 * 
 * Creates and shows a context menu with edit and delete options.
 * 
 * @param pos The position where the context menu was requested
 */
void TimeTrackerWidget::onContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = m_recentEntriesView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    
    QMenu contextMenu(this);
    
    QAction* editAction = contextMenu.addAction("Edit");
    QAction* deleteAction = contextMenu.addAction("Delete");
    
    connect(editAction, &QAction::triggered, this, &TimeTrackerWidget::onEditEntryClicked);
    connect(deleteAction, &QAction::triggered, this, &TimeTrackerWidget::onDeleteEntryClicked);
    
    contextMenu.exec(m_recentEntriesView->mapToGlobal(pos));
}

/**
 * @brief Update the project combo box
 * 
 * Refreshes the project combo box with the current list of active projects.
 * Preserves the current selection if possible.
 */
void TimeTrackerWidget::updateProjectComboBox()
{
    QSignalBlocker blocker(m_projectComboBox);
    
    // Store the current project ID before clearing
    QString currentProjectId;
    if (m_projectComboBox->currentIndex() >= 0) {
        currentProjectId = m_projectComboBox->currentData().toString();
    }
    
    // If we're tracking a project, make sure we use that ID
    if (m_isTracking && m_controller.isTimerRunning()) {
        currentProjectId = m_controller.getCurrentProjectId();
    }
    
    m_projectComboBox->clear();
    
    QList<Project> projects = ProjectController::instance().getActiveProjects();
    if (projects.isEmpty()) {
        qDebug() << "No active projects found to populate ComboBox";
    } else {
        qDebug() << "Found" << projects.size() << "active projects to populate ComboBox";
    }
    
    for (const Project& project : projects) {
        m_projectComboBox->addItem(project.name(), project.id());
        
        // Set item color indicator
        int index = m_projectComboBox->count() - 1;
        QPixmap pixmap(16, 16);
        pixmap.fill(project.color());
        m_projectComboBox->setItemIcon(index, QIcon(pixmap));
    }
    
    // Restore selection if possible
    if (!currentProjectId.isEmpty()) {
        int index = m_projectComboBox->findData(currentProjectId);
        if (index >= 0) {
            m_projectComboBox->setCurrentIndex(index);
            qDebug() << "Restored project selection:" << currentProjectId;
        } else {
            qDebug() << "Could not find project with ID:" << currentProjectId;
        }
    }
}

/**
 * @brief Update the summary information
 * 
 * Updates the labels showing today's total, weekly total, and most tracked project.
 */
void TimeTrackerWidget::updateSummary()
{
    // Update today's total
    int todayTotal = m_controller.getTodayTotal();
    m_todayTotalLabel->setText("Today: " + TimeTrackingController::formatDuration(todayTotal, "h:mm"));
    
    // Update this week's total
    int weekTotal = m_controller.getWeekTotal();
    m_weekTotalLabel->setText("This Week: " + TimeTrackingController::formatDuration(weekTotal, "h:mm"));
    
    // Update most tracked project
    QDate startOfWeek = QDate::currentDate().addDays(-(QDate::currentDate().dayOfWeek() - 1));
    QString mostTrackedProjectId = m_controller.getMostTrackedProject(startOfWeek, QDate::currentDate());
    
    if (!mostTrackedProjectId.isEmpty()) {
        Project project = ProjectController::instance().getProject(mostTrackedProjectId);
        m_mostTrackedLabel->setText("Most Tracked: " + project.name());
        
        // Add color indicator
        QString colorStyle = QString("QLabel { background-color: rgba(%1, %2, %3, 50); border-radius: 3px; padding: 3px; }")
                              .arg(project.color().red())
                              .arg(project.color().green())
                              .arg(project.color().blue());
        m_mostTrackedLabel->setStyleSheet(colorStyle);
    } else {
        m_mostTrackedLabel->setText("Most Tracked: None");
        m_mostTrackedLabel->setStyleSheet("");
    }
}

/**
 * @brief Handle timer started signal from controller
 * 
 * Updates the UI to reflect that time tracking has started.
 * 
 * @param projectId The ID of the project being tracked
 */
void TimeTrackerWidget::onTimerStarted(const QString& projectId)
{
    m_isTracking = true;
    m_startStopButton->setText("Stop");
    
    // Select the project in the combo box
    int index = m_projectComboBox->findData(projectId);
    if (index >= 0) {
        QSignalBlocker blocker(m_projectComboBox);
        m_projectComboBox->setCurrentIndex(index);
    }
    
    updateTimerDisplay(0);
}

/**
 * @brief Handle timer stopped signal from controller
 * 
 * Updates the UI to reflect that time tracking has stopped and updates the summary.
 * 
 * @param duration The duration of the time entry that was created
 */
void TimeTrackerWidget::onTimerStopped(int duration)
{
    m_isTracking = false;
    m_startStopButton->setText("Start");
    updateTimerDisplay(0);
    updateSummary();
}

/**
 * @brief Handle time entry added signal from controller
 * 
 * Updates the summary information when a time entry is added.
 * 
 * @param entry The TimeEntry that was added
 */
void TimeTrackerWidget::onTimeEntryAdded(const TimeEntry& entry)
{
    updateSummary();
}

/**
 * @brief Handle time entry updated signal from controller
 * 
 * Updates the summary information when a time entry is updated.
 * 
 * @param entry The TimeEntry that was updated
 */
void TimeTrackerWidget::onTimeEntryUpdated(const TimeEntry& entry)
{
    updateSummary();
}

/**
 * @brief Handle time entry deleted signal from controller
 * 
 * Updates the summary information when a time entry is deleted.
 * 
 * @param id The ID of the TimeEntry that was deleted
 */
void TimeTrackerWidget::onTimeEntryDeleted(const QString& id)
{
    updateSummary();
}