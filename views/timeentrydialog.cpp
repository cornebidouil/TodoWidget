/**
 * @file timeentrydialog.cpp
 * @brief Implementation of the TimeEntryDialog class
 * 
 * The TimeEntryDialog class provides a dialog for creating and editing TimeEntry objects.
 * It allows users to select a project, set start and end times, specify duration,
 * and add notes. The class can be used in two modes: add mode for creating new time entries
 * and edit mode for modifying existing ones.
 * 
 * @author Cornebidouil
 * @date Last updated: April 30, 2025
 */

#include "timeentrydialog.h"
#include "../controllers/projectcontroller.h"
#include "../controllers/timetrackingcontroller.h"
#include <QMessageBox>
#include <QDebug>
#include <QUuid>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>

/**
 * @brief Constructor for creating a new time entry
 * 
 * Initializes the dialog in "add" mode with default values.
 * By default, sets the start time to one hour ago and end time to now.
 * 
 * @param parent The parent widget
 */
TimeEntryDialog::TimeEntryDialog(QWidget* parent)
    : QDialog(parent)
    , m_isEditMode(false)
    , m_entryId(QUuid::createUuid().toString().remove('{').remove('}'))
    , m_updatingControls(false)
{
    setWindowTitle("Add Time Entry");
    setupUi();
    setupConnections();
    populateProjects();
    
    // Set default values
    m_startTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(-3600)); // 1 hour ago
    m_endTimeEdit->setDateTime(QDateTime::currentDateTime());
    onEndTimeChanged(); // Update duration
}

/**
 * @brief Constructor for editing an existing time entry
 * 
 * Initializes the dialog in "edit" mode with values from the provided time entry.
 * 
 * @param entry The TimeEntry object to edit
 * @param parent The parent widget
 */
TimeEntryDialog::TimeEntryDialog(const TimeEntry& entry, QWidget* parent)
    : QDialog(parent)
    , m_isEditMode(true)
    , m_entryId(entry.id())
    , m_updatingControls(false)
{
    setWindowTitle("Edit Time Entry");
    setupUi();
    setupConnections();
    populateProjects();
    
    // Populate with entry data
    setTimeEntry(entry);
}

/**
 * @brief Destructor for TimeEntryDialog
 */
TimeEntryDialog::~TimeEntryDialog()
{
}

/**
 * @brief Set up the user interface
 * 
 * Creates and arranges all UI elements for the dialog.
 */
void TimeEntryDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout();
    
    // Project selection
    m_projectComboBox = new QComboBox(this);
    formLayout->addRow("Project:", m_projectComboBox);
    
    // Project display
    m_projectLabel = new QLabel(this);
    formLayout->addRow("", m_projectLabel);
    
    // Time range group
    QGroupBox* timeGroup = new QGroupBox("Time Range", this);
    QVBoxLayout* timeLayout = new QVBoxLayout(timeGroup);
    
    QFormLayout* timeFormLayout = new QFormLayout();
    m_startTimeEdit = new QDateTimeEdit(this);
    m_startTimeEdit->setCalendarPopup(true);
    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    timeFormLayout->addRow("Start Time:", m_startTimeEdit);
    
    m_endTimeEdit = new QDateTimeEdit(this);
    m_endTimeEdit->setCalendarPopup(true);
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    timeFormLayout->addRow("End Time:", m_endTimeEdit);
    
    timeLayout->addLayout(timeFormLayout);
    
    // Duration group
    QGroupBox* durationGroup = new QGroupBox("Duration", this);
    QVBoxLayout* durationLayout = new QVBoxLayout(durationGroup);
    
    m_autoDurationCheck = new QCheckBox("Calculate automatically", this);
    m_autoDurationCheck->setChecked(true);
    durationLayout->addWidget(m_autoDurationCheck);
    
    QHBoxLayout* durationInputLayout = new QHBoxLayout();
    m_hoursSpinBox = new QSpinBox(this);
    m_hoursSpinBox->setRange(0, 99);
    m_hoursSpinBox->setSuffix(" h");
    m_hoursSpinBox->setEnabled(false);
    
    m_minutesSpinBox = new QSpinBox(this);
    m_minutesSpinBox->setRange(0, 59);
    m_minutesSpinBox->setSuffix(" m");
    m_minutesSpinBox->setEnabled(false);
    
    m_secondsSpinBox = new QSpinBox(this);
    m_secondsSpinBox->setRange(0, 59);
    m_secondsSpinBox->setSuffix(" s");
    m_secondsSpinBox->setEnabled(false);
    
    durationInputLayout->addWidget(m_hoursSpinBox);
    durationInputLayout->addWidget(m_minutesSpinBox);
    durationInputLayout->addWidget(m_secondsSpinBox);
    
    durationLayout->addLayout(durationInputLayout);
    
    // Notes
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setPlaceholderText("Enter notes about this time entry (optional)");
    m_notesEdit->setMaximumHeight(100);
    
    // Buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    
    // Add everything to main layout
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(timeGroup);
    mainLayout->addWidget(durationGroup);
    mainLayout->addWidget(new QLabel("Notes:", this));
    mainLayout->addWidget(m_notesEdit);
    mainLayout->addWidget(m_buttonBox);
    
    setLayout(mainLayout);
    setMinimumWidth(400);
}

/**
 * @brief Set up signal/slot connections
 * 
 * Connects UI elements to their respective slots.
 */
void TimeEntryDialog::setupConnections()
{
    connect(m_projectComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &TimeEntryDialog::onProjectChanged);
    
    connect(m_startTimeEdit, &QDateTimeEdit::dateTimeChanged, 
            this, &TimeEntryDialog::onStartTimeChanged);
    
    connect(m_endTimeEdit, &QDateTimeEdit::dateTimeChanged, 
            this, &TimeEntryDialog::onEndTimeChanged);
    
    connect(m_autoDurationCheck, &QCheckBox::toggled, 
            this, &TimeEntryDialog::onAutoDurationToggled);
    
    connect(m_hoursSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &TimeEntryDialog::onDurationChanged);
    
    connect(m_minutesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &TimeEntryDialog::onDurationChanged);
    
    connect(m_secondsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &TimeEntryDialog::onDurationChanged);
    
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &TimeEntryDialog::validate);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

/**
 * @brief Populate the projects combo box
 * 
 * Loads active projects from the ProjectController and adds them to the combo box.
 */
void TimeEntryDialog::populateProjects()
{
    m_projectComboBox->clear();
    
    QList<Project> projects = ProjectController::instance().getActiveProjects();
    for (const Project& project : projects) {
        m_projectComboBox->addItem(project.name(), project.id());
        
        // Set item color indicator
        int index = m_projectComboBox->count() - 1;
        QPixmap pixmap(16, 16);
        pixmap.fill(project.color());
        m_projectComboBox->setItemIcon(index, QIcon(pixmap));
    }
    
    if (m_projectComboBox->count() > 0) {
        onProjectChanged(0);
    }
}

/**
 * @brief Handle project selection change
 * 
 * Updates the project description display when the selected project changes.
 * 
 * @param index The index of the newly selected project in the combo box
 */
void TimeEntryDialog::onProjectChanged(int index)
{
    if (index >= 0) {
        QString projectId = m_projectComboBox->itemData(index).toString();
        updateProjectDisplay(projectId);
    } else {
        m_projectLabel->setText("No project selected");
        m_projectLabel->setStyleSheet("");
    }
}

/**
 * @brief Update the project description display
 * 
 * Updates the label that shows the project description and sets its background
 * color to match the project color.
 * 
 * @param projectId The ID of the project to display
 */
void TimeEntryDialog::updateProjectDisplay(const QString& projectId)
{
    Project project = ProjectController::instance().getProject(projectId);
    if (project.id().isEmpty()) {
        m_projectLabel->setText("No project selected");
        m_projectLabel->setStyleSheet("");
        return;
    }
    
    m_projectLabel->setText(project.description());
    
    // Set background color based on project color
    QColor color = project.color();
    color.setAlpha(80);
    QString styleSheet = QString("QLabel { background-color: %1; border-radius: 3px; padding: 3px; }").arg(color.name(QColor::HexArgb));
    m_projectLabel->setStyleSheet(styleSheet);
}

/**
 * @brief Handle duration input changes
 * 
 * Updates the end time based on start time and manual duration when
 * the duration fields are changed and automatic calculation is disabled.
 */
void TimeEntryDialog::onDurationChanged()
{
    if (m_updatingControls || m_autoDurationCheck->isChecked()) {
        return;
    }
    
    m_updatingControls = true;
    
    // Calculate new end time based on start time and duration
    int durationSecs = m_hoursSpinBox->value() * 3600 + 
                      m_minutesSpinBox->value() * 60 + 
                      m_secondsSpinBox->value();
    
    QDateTime newEndTime = m_startTimeEdit->dateTime().addSecs(durationSecs);
    m_endTimeEdit->setDateTime(newEndTime);
    
    m_updatingControls = false;
}

/**
 * @brief Handle start time changes
 * 
 * Updates either the duration or end time when the start time changes,
 * depending on whether automatic duration calculation is enabled.
 */
void TimeEntryDialog::onStartTimeChanged()
{
    if (m_updatingControls) {
        return;
    }
    
    m_updatingControls = true;
    
    if (m_autoDurationCheck->isChecked()) {
        // Update duration
        int durationSecs = m_startTimeEdit->dateTime().secsTo(m_endTimeEdit->dateTime());
        if (durationSecs < 0) {
            // End time is before start time, adjust end time
            m_endTimeEdit->setDateTime(m_startTimeEdit->dateTime());
            durationSecs = 0;
        }
        
        int hours = durationSecs / 3600;
        int minutes = (durationSecs % 3600) / 60;
        int seconds = durationSecs % 60;
        
        m_hoursSpinBox->setValue(hours);
        m_minutesSpinBox->setValue(minutes);
        m_secondsSpinBox->setValue(seconds);
    } else {
        // Fixed duration, update end time
        int durationSecs = m_hoursSpinBox->value() * 3600 + 
                          m_minutesSpinBox->value() * 60 + 
                          m_secondsSpinBox->value();
        
        QDateTime newEndTime = m_startTimeEdit->dateTime().addSecs(durationSecs);
        m_endTimeEdit->setDateTime(newEndTime);
    }
    
    m_updatingControls = false;
}

/**
 * @brief Handle end time changes
 * 
 * Updates the duration when the end time changes and automatic
 * duration calculation is enabled.
 */
void TimeEntryDialog::onEndTimeChanged()
{
    if (m_updatingControls || !m_autoDurationCheck->isChecked()) {
        return;
    }
    
    m_updatingControls = true;
    
    // Update duration based on start and end times
    int durationSecs = m_startTimeEdit->dateTime().secsTo(m_endTimeEdit->dateTime());
    if (durationSecs < 0) {
        // End time is before start time, adjust end time
        m_endTimeEdit->setDateTime(m_startTimeEdit->dateTime());
        durationSecs = 0;
    }
    
    int hours = durationSecs / 3600;
    int minutes = (durationSecs % 3600) / 60;
    int seconds = durationSecs % 60;
    
    m_hoursSpinBox->setValue(hours);
    m_minutesSpinBox->setValue(minutes);
    m_secondsSpinBox->setValue(seconds);
    
    m_updatingControls = false;
}

/**
 * @brief Handle automatic duration checkbox toggle
 * 
 * Enables or disables the duration input fields and updates either
 * the duration or end time based on the new state.
 * 
 * @param automatic Whether automatic duration calculation is enabled
 */
void TimeEntryDialog::onAutoDurationToggled(bool automatic)
{
    m_hoursSpinBox->setEnabled(!automatic);
    m_minutesSpinBox->setEnabled(!automatic);
    m_secondsSpinBox->setEnabled(!automatic);
    
    if (automatic) {
        onEndTimeChanged();
    } else {
        onDurationChanged();
    }
}

/**
 * @brief Validate user input
 * 
 * Checks if the input is valid and accepts the dialog if it is.
 * Validates that a project is selected, start time is before end time,
 * and duration is non-zero.
 */
void TimeEntryDialog::validate()
{
    // Check if a project is selected
    if (m_projectComboBox->currentIndex() < 0) {
        QMessageBox::warning(this, "Missing Project", "Please select a project for this time entry.");
        return;
    }
    
    // Check if start time is before end time
    if (m_startTimeEdit->dateTime() >= m_endTimeEdit->dateTime()) {
        QMessageBox::warning(this, "Invalid Time Range", "The start time must be before the end time.");
        return;
    }
    
    // Check if duration is non-zero
    int durationSecs = m_hoursSpinBox->value() * 3600 + 
                      m_minutesSpinBox->value() * 60 + 
                      m_secondsSpinBox->value();
    
    if (durationSecs == 0) {
        QMessageBox::warning(this, "Zero Duration", "The time entry must have a non-zero duration.");
        return;
    }
    
    accept();
}

/**
 * @brief Get the time entry with user-entered values
 * 
 * Creates and returns a TimeEntry object with the values entered by the user.
 * 
 * @return A TimeEntry object with the user-entered values
 */
TimeEntry TimeEntryDialog::getTimeEntry() const
{
    QString projectId = m_projectComboBox->currentData().toString();
    QDateTime startTime = m_startTimeEdit->dateTime();
    QDateTime endTime = m_endTimeEdit->dateTime();
    
    int durationSecs = m_hoursSpinBox->value() * 3600 + 
                      m_minutesSpinBox->value() * 60 + 
                      m_secondsSpinBox->value();
    
    QString notes = m_notesEdit->toPlainText();
    
    TimeEntry entry(projectId, startTime, endTime, durationSecs, notes);
    entry.setId(m_entryId);
    
    return entry;
}

/**
 * @brief Set the dialog values from a TimeEntry object
 * 
 * Populates the UI elements with values from the provided TimeEntry object.
 * 
 * @param entry The TimeEntry object to get values from
 */
void TimeEntryDialog::setTimeEntry(const TimeEntry& entry)
{
    m_entryId = entry.id();
    
    // Set project
    int projectIndex = m_projectComboBox->findData(entry.projectId());
    if (projectIndex >= 0) {
        m_projectComboBox->setCurrentIndex(projectIndex);
    }
    
    // Set times
    m_startTimeEdit->setDateTime(entry.startTime());
    
    if (entry.endTime().isValid()) {
        m_endTimeEdit->setDateTime(entry.endTime());
    } else {
        m_endTimeEdit->setDateTime(QDateTime::currentDateTime());
    }
    
    // Set duration
    int durationSecs = entry.duration();
    int hours = durationSecs / 3600;
    int minutes = (durationSecs % 3600) / 60;
    int seconds = durationSecs % 60;
    
    m_hoursSpinBox->setValue(hours);
    m_minutesSpinBox->setValue(minutes);
    m_secondsSpinBox->setValue(seconds);
    
    // Set notes
    m_notesEdit->setPlainText(entry.notes());
    
    // If duration was manually set, switch to manual mode
    if (durationSecs != entry.startTime().secsTo(entry.endTime())) {
        m_autoDurationCheck->setChecked(false);
    }
}
