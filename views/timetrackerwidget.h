/**
 * @file timetrackerwidget.h
 * @brief Definition of the TimeTrackerWidget class
 * 
 * This file defines the TimeTrackerWidget class which provides a user interface
 * for time tracking functionality. It includes a timer display, task selection,
 * start/stop controls, and a list of recent time entries.
 * 
 * @author Cornebidouil
 * @date Last updated: April 29, 2025
 */

#pragma once

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include "../models/timeentrymodel.h"
#include "../controllers/timetrackingcontroller.h"

class TimeEntryDialog;
class TimeReportsDialog;

/**
 * @class TimeTrackerWidget
 * @brief Widget for time tracking functionality
 * 
 * The TimeTrackerWidget class provides a user interface for tracking time spent on tasks.
 * It includes controls for starting and stopping a timer, selecting a task to track,
 * displaying recent time entries, and accessing time reports.
 */
class TimeTrackerWidget : public QWidget {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor
     * 
     * Creates a new TimeTrackerWidget.
     * 
     * @param parent Parent widget
     */
    explicit TimeTrackerWidget(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~TimeTrackerWidget();

public slots:
    /**
     * @brief Update project combo box
     * 
     * Refreshes the list of projects in the combo box.
     */
    void updateProjectComboBox();

private slots:
    /**
     * @brief Handle start/stop button click
     * 
     * Starts or stops the timer based on current state.
     */
    void onStartStopClicked();
    
    /**
     * @brief Handle project selection change
     * 
     * Updates the current project being tracked.
     * 
     * @param index Index of the selected project in the combo box
     */
    void onProjectSelectionChanged(int index);
    
    /**
     * @brief Handle timer tick
     * 
     * Updates the timer display when the timer ticks.
     * 
     * @param elapsed Elapsed time in seconds
     */
    void onTimerTick(int elapsed);
    
    /**
     * @brief Handle add manual entry button click
     * 
     * Opens a dialog for adding a time entry manually.
     */
    void onAddManualEntryClicked();
    
    /**
     * @brief Handle reports button click
     * 
     * Opens the time reports dialog.
     */
    void onReportsClicked();
    
    /**
     * @brief Handle edit entry action
     * 
     * Opens a dialog for editing the selected time entry.
     */
    void onEditEntryClicked();
    
    /**
     * @brief Handle delete entry action
     * 
     * Deletes the selected time entry after confirmation.
     */
    void onDeleteEntryClicked();
    
    /**
     * @brief Handle entry double-click
     * 
     * Opens the edit dialog for the double-clicked entry.
     * 
     * @param index Model index of the double-clicked entry
     */
    void onEntryDoubleClicked(const QModelIndex& index);
    
    /**
     * @brief Handle context menu request
     * 
     * Shows a context menu for the time entry list.
     * 
     * @param pos Position where the context menu should appear
     */
    void onContextMenuRequested(const QPoint& pos);
    
    /**
     * @brief Update summary labels
     * 
     * Updates the summary information displayed at the bottom of the widget.
     */
    void updateSummary();
    
    /**
     * @brief Handle timer started signal
     * 
     * Updates UI when a timer is started.
     * 
     * @param projectId ID of the project being tracked
     */
    void onTimerStarted(const QString& projectId);
    
    /**
     * @brief Handle timer stopped signal
     * 
     * Updates UI when a timer is stopped.
     * 
     * @param duration Duration of the tracked time in seconds
     */
    void onTimerStopped(int duration);
    
    /**
     * @brief Handle time entry added signal
     * 
     * Updates the UI when a new time entry is added.
     * 
     * @param entry The added time entry
     */
    void onTimeEntryAdded(const TimeEntry& entry);
    
    /**
     * @brief Handle time entry updated signal
     * 
     * Updates the UI when a time entry is updated.
     * 
     * @param entry The updated time entry
     */
    void onTimeEntryUpdated(const TimeEntry& entry);
    
    /**
     * @brief Handle time entry deleted signal
     * 
     * Updates the UI when a time entry is deleted.
     * 
     * @param id ID of the deleted time entry
     */
    void onTimeEntryDeleted(const QString& id);

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
     * @brief Update the timer display
     * 
     * Updates the timer label with the formatted elapsed time.
     * 
     * @param seconds Elapsed time in seconds
     */
    void updateTimerDisplay(int seconds);

    // UI elements
    QComboBox* m_projectComboBox;      ///< Combo box for selecting a project
    QPushButton* m_startStopButton;    ///< Button for starting/stopping the timer
    QLabel* m_timerLabel;              ///< Label displaying the current timer value
    QPushButton* m_addManualButton;    ///< Button for adding a manual time entry
    QListView* m_recentEntriesView;    ///< List view for displaying recent time entries
    QPushButton* m_reportsButton;      ///< Button for opening the reports dialog
    QLabel* m_todayTotalLabel;         ///< Label showing today's total tracked time
    QLabel* m_weekTotalLabel;          ///< Label showing this week's total tracked time
    QLabel* m_mostTrackedLabel;        ///< Label showing the most tracked project
    
    // Models and controllers
    TimeEntryModel* m_timeEntryModel;                 ///< Model for time entries
    TimeTrackingController& m_controller;             ///< Controller for time tracking operations
    
    // State
    bool m_isTracking;                 ///< Flag indicating if tracking is active
    
    // Dialogs
    TimeEntryDialog* m_timeEntryDialog;     ///< Dialog for editing time entries
    TimeReportsDialog* m_reportsDialog;     ///< Dialog for viewing reports
};
