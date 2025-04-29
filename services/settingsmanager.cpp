/**
 * @file settingsmanager.cpp
 * @brief Implementation of the SettingsManager class
 * 
 * This file implements the SettingsManager class which provides a singleton interface
 * for managing application settings. It handles loading, saving, and accessing user
 * preferences such as window position, size, opacity, startup behavior, and notification
 * settings.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#include "settingsmanager.h"
#include <QApplication>
#include <QScreen>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

/**
 * @brief Get singleton instance
 * 
 * Returns a reference to the singleton SettingsManager instance.
 * Creates the instance if it doesn't exist yet.
 * 
 * @return SettingsManager& Reference to the singleton instance
 */
SettingsManager& SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

/**
 * @brief Constructor
 * 
 * Creates a new SettingsManager instance with default settings.
 * Initializes the settings file in the application data directory
 * and loads existing settings if available.
 * 
 * @param parent Optional parent QObject
 */
SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent),
      m_settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/settings.ini", QSettings::IniFormat),
      m_alwaysOnTop(false),
      m_opacity(0.9),
      m_windowPosition(QPoint(100, 100)),
      m_windowSize(QSize(300, 400)),
      m_startWithWindows(false),
      m_startMinimized(false),
      m_enableNotifications(true),
      m_defaultCategoryId("")
{
    // Get application data path (same as used in DatabaseManager)
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    
    // Create directory if it doesn't exist
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Load settings from file
    load();
}

/**
 * @brief Destructor
 * 
 * Saves current settings to the settings file before destruction.
 */
SettingsManager::~SettingsManager()
{
    save();
}

/**
 * @brief Get always-on-top setting
 * 
 * @return bool True if the window should always stay on top, false otherwise
 */
bool SettingsManager::alwaysOnTop() const
{
    return m_alwaysOnTop;
}

/**
 * @brief Set always-on-top setting
 * 
 * Updates the always-on-top setting and saves the change.
 * 
 * @param value True to make the window always on top, false otherwise
 */
void SettingsManager::setAlwaysOnTop(bool value)
{
    if (m_alwaysOnTop != value) {
        m_alwaysOnTop = value;
        save();
    }
}

/**
 * @brief Get window opacity
 * 
 * @return double Opacity value (0.0 to 1.0, where 1.0 is fully opaque)
 */
double SettingsManager::opacity() const
{
    return m_opacity;
}

/**
 * @brief Set window opacity
 * 
 * Updates the window opacity setting and saves the change.
 * 
 * @param value Opacity value (0.0 to 1.0, where 1.0 is fully opaque)
 */
void SettingsManager::setOpacity(double value)
{
    if (m_opacity != value) {
        m_opacity = value;
        save();
    }
}

/**
 * @brief Get window position
 *
 * @return QPoint The position of the application window
 */
QPoint SettingsManager::windowPosition() const
{
    return m_windowPosition;
}

/**
 * @brief Set window position
 * 
 * Updates the window position setting and saves the change.
 * 
 * @param position The new position for the application window
 */
void SettingsManager::setWindowPosition(const QPoint& position)
{
    if (m_windowPosition != position) {
        m_windowPosition = position;
        save();
    }
}

/**
 * @brief Get window size
 * 
 * @return QSize The size of the application window
 */
QSize SettingsManager::windowSize() const
{
    return m_windowSize;
}

/**
 * @brief Set window size
 * 
 * Updates the window size setting and saves the change.
 * 
 * @param size The new size for the application window
 */
void SettingsManager::setWindowSize(const QSize& size)
{
    if (m_windowSize != size) {
        m_windowSize = size;
        save();
    }
}

/**
 * @brief Get start with Windows setting
 * 
 * @return bool True if the application should start with Windows, false otherwise
 */
bool SettingsManager::startWithWindows() const
{
    return m_startWithWindows;
}

/**
 * @brief Set start with Windows setting
 * 
 * Updates the setting for starting the application with Windows and
 * modifies the Windows registry to enable/disable autostart.
 * 
 * @param value True to start with Windows, false otherwise
 */
void SettingsManager::setStartWithWindows(bool value) {
    if (m_startWithWindows != value) {
        m_startWithWindows = value;
        // Set up or remove autostart entry in Windows registry
        QSettings startupSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                                 QSettings::NativeFormat);
        if (value) {
            QString appPath = QApplication::applicationFilePath();
            appPath = appPath.replace('/', '\\');

            // Get the application directory to use as working directory
            QFileInfo appInfo(appPath);
            QString appDir = appInfo.absolutePath();

            // Use the start command to specify both the application and working directory
            QString startCommand = QString("cmd /c start /D \"%1\" \"\" \"%2\"").arg(appDir, appPath);

            startupSettings.setValue("TodoWidget", startCommand);
        } else {
            startupSettings.remove("TodoWidget");
        }
        save();
    }
}

/**
 * @brief Get start minimized setting
 * 
 * @return bool True if the application should start minimized, false otherwise
 */
bool SettingsManager::startMinimized() const
{
    return m_startMinimized;
}

/**
 * @brief Set start minimized setting
 * 
 * Updates the setting for starting the application minimized.
 * 
 * @param minimized True to start minimized, false otherwise
 */
void SettingsManager::setStartMinimized(bool minimized)
{
    m_startMinimized = minimized;
}

/**
 * @brief Get notifications enabled setting
 * 
 * @return bool True if notifications are enabled, false otherwise
 */
bool SettingsManager::enableNotifications() const
{
    return m_enableNotifications;
}

/**
 * @brief Set notifications enabled setting
 * 
 * Updates the setting for enabling/disabling notifications and saves the change.
 * 
 * @param value True to enable notifications, false otherwise
 */
void SettingsManager::setEnableNotifications(bool value)
{
    if (m_enableNotifications != value) {
        m_enableNotifications = value;
        save();
    }
}

/**
 * @brief Get default category ID
 * 
 * @return QString The ID of the default category for new tasks
 */
QString SettingsManager::defaultCategoryId() const
{
    return m_defaultCategoryId;
}

/**
 * @brief Set default category ID
 * 
 * Updates the default category ID for new tasks and saves the change.
 * 
 * @param id The ID of the category to use as default
 */
void SettingsManager::setDefaultCategoryId(const QString& id)
{
    if (m_defaultCategoryId != id) {
        m_defaultCategoryId = id;
        save();
    }
}

/**
 * @brief Load settings from file
 * 
 * Loads all application settings from the settings file.
 * If loading fails, settings are reset to defaults.
 */
void SettingsManager::load()
{
    try {
        m_alwaysOnTop = m_settings.value("Window/AlwaysOnTop", false).toBool();
        m_opacity = m_settings.value("Window/Opacity", 1.0).toDouble();
        m_windowPosition = m_settings.value("Window/Position", QPoint(100, 100)).toPoint();
        m_windowSize = m_settings.value("Window/Size", QSize(300, 400)).toSize();

        m_startWithWindows = m_settings.value("Startup/StartWithWindows", false).toBool();
        m_startMinimized = m_settings.value("Startup/StartMinimized", false).toBool();

        m_defaultCategoryId = m_settings.value("Categories/DefaultCategory", "").toString();

        m_enableNotifications = m_settings.value("Notifications/Enable", true).toBool();
        m_notifyDueSoon = m_settings.value("Notifications/NotifyDueSoon", true).toBool();
        m_notifyOverdue = m_settings.value("Notifications/NotifyOverdue", true).toBool();
    } catch (const std::exception& e) {
        qWarning() << "Exception during settings loading:" << e.what();
        // Reset to defaults if loading fails
        resetToDefaults();
    }
}

/**
 * @brief Save settings to file
 * 
 * Saves all application settings to the settings file and
 * ensures the file is synced to disk.
 */
void SettingsManager::save()
{
    m_settings.setValue("Window/AlwaysOnTop", m_alwaysOnTop);
    m_settings.setValue("Window/Opacity", m_opacity);
    m_settings.setValue("Window/Position", m_windowPosition);
    m_settings.setValue("Window/Size", m_windowSize);

    m_settings.setValue("Startup/StartWithWindows", m_startWithWindows);
    m_settings.setValue("Startup/StartMinimized", m_startMinimized);

    m_settings.setValue("Categories/DefaultCategory", m_defaultCategoryId);

    m_settings.setValue("Notifications/Enable", m_enableNotifications);
    m_settings.setValue("Notifications/NotifyDueSoon", m_notifyDueSoon);
    m_settings.setValue("Notifications/NotifyOverdue", m_notifyOverdue);

    m_settings.sync();
}

/**
 * @brief Reset settings to defaults
 * 
 * Resets all settings to their default values and saves the changes.
 * This is called when loading settings fails or when the user explicitly
 * requests a reset.
 */
void SettingsManager::resetToDefaults()
{
    // Reset window settings
    m_alwaysOnTop = false;
    m_opacity = 1.0;
    m_windowPosition = QPoint(100, 100);
    m_windowSize = QSize(300, 400);

    // Reset startup settings
    m_startWithWindows = false;
    m_startMinimized = false;

    // Reset category settings
    m_defaultCategoryId = "";

    // Reset notification settings
    m_enableNotifications = true;
    m_notifyDueSoon = true;
    m_notifyOverdue = true;

    // Save the default settings
    save();

    qInfo() << "Settings have been reset to defaults";
}

/**
 * @brief Get notify-due-soon setting
 * 
 * @return bool True if notifications for tasks due soon are enabled, false otherwise
 */
bool SettingsManager::notifyDueSoon() const
{
    return m_notifyDueSoon;
}

/**
 * @brief Set notify-due-soon setting
 * 
 * Updates the setting for showing notifications for tasks due soon.
 * 
 * @param notify True to enable due-soon notifications, false otherwise
 */
void SettingsManager::setNotifyDueSoon(bool notify)
{
    m_notifyDueSoon = notify;
}

/**
 * @brief Get notify-overdue setting
 * 
 * @return bool True if notifications for overdue tasks are enabled, false otherwise
 */
bool SettingsManager::notifyOverdue() const
{
    return m_notifyOverdue;
}

/**
 * @brief Set notify-overdue setting
 * 
 * Updates the setting for showing notifications for overdue tasks.
 * 
 * @param notify True to enable overdue notifications, false otherwise
 */
void SettingsManager::setNotifyOverdue(bool notify)
{
    m_notifyOverdue = notify;
}
