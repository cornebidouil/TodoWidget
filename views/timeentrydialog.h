/**
 * @file timeentrydialog.h
 * @brief Definition of the TimeEntryDialog class
 * 
 * This file defines the TimeEntryDialog class which provides a dialog
 * for creating and editing time entries.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#pragma once

#include <QDialog>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include "../models/timeentry.h"

/**
 * @class TimeEntryDialog
 * @brief Dialog for creating and editing time entries
 * 
 * The TimeEntryDialog class provides a form for entering time entry details,
 * including project selection, start/end times, duration, and notes.
 */
class TimeEntryDialog : public QDialog {
    Q_OBJECT
    
public:
    /**
     * @brief Constructor for creating a new time entry
     * 
     * Creates a dialog for adding a new time entry.
     * 
     * @param parent Parent widget
     */
    explicit TimeEntryDialog(QWidget* parent = nullptr);
    
    /**
     * @brief Constructor for editing an existing time entry
     * 
     * Creates a dialog pre-populated with the details of an existing time entry.
     * 
     * @param entry The time entry to edit
     * @param parent Parent widget
     */
    TimeEntryDialog(const TimeEntry& entry, QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~TimeEntryDialog();
    
    /**
     * @brief Get the time entry from the dialog
     * 
     * Constructs a TimeEntry object from the current form values.
     * 
     * @return TimeEntry The time entry with values from the form
     */
    TimeEntry getTimeEntry() const;
    
    /**
     * @brief Set the time entry to edit
     * 
     * Populates the form with values from an existing time entry.
     * 
     * @param entry The time entry to edit
     */
    void setTimeEntry(const TimeEntry& entry);

private slots:
    /**
     * @brief Handle project selection change
     * 
     * Updates the project color display when a different project is selected.
     * 
     * @param index Index of the selected project in the combo box
     */
    void onProjectChanged(int index);
    
    /**
     * @brief Handle duration manual change
     * 
     * Updates the end time when the duration is changed manually.
     */
    void onDurationChanged();
    
    /**
     * @brief Handle start time change
     * 
     * Updates the duration when the start time is changed.
     */
    void onStartTimeChanged();
    
    /**
     * @brief Handle end time change
     * 
     * Updates the duration when the end time is changed.
     */
    void onEndTimeChanged();
    
    /**
     * @brief Handle automatic duration toggle
     * 
     * Enables or disables manual duration editing.
     * 
     * @param automatic Whether to calculate duration automatically
     */
    void onAutoDurationToggled(bool automatic);
    
    /**
     * @brief Validate the form inputs
     * 
     * Validates the form inputs before accepting the dialog.
     */
    void validate();

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
     * @brief Populate the project combo box
     * 
     * Fills the project combo box with available projects.
     */
    void populateProjects();
    
    /**
     * @brief Update the project color display
     * 
     * Updates the project label based on the selected project.
     * 
     * @param projectId ID of the selected project
     */
    void updateProjectDisplay(const QString& projectId);

    // UI elements
    QComboBox* m_projectComboBox;       ///< Combo box for selecting a project
    QDateTimeEdit* m_startTimeEdit;     ///< Date/time editor for start time
    QDateTimeEdit* m_endTimeEdit;       ///< Date/time editor for end time
    QCheckBox* m_autoDurationCheck;     ///< Checkbox for automatic duration calculation
    QSpinBox* m_hoursSpinBox;           ///< Spin box for hours
    QSpinBox* m_minutesSpinBox;         ///< Spin box for minutes
    QSpinBox* m_secondsSpinBox;         ///< Spin box for seconds
    QTextEdit* m_notesEdit;             ///< Text editor for notes
    QLabel* m_projectLabel;             ///< Label showing the project color
    QDialogButtonBox* m_buttonBox;      ///< Dialog button box
    
    // State
    bool m_isEditMode;                  ///< Flag indicating if editing an existing entry
    QString m_entryId;                  ///< ID of the entry being edited
    bool m_updatingControls;            ///< Flag to prevent recursive signal handling
};
