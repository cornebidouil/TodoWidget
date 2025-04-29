/**
 * @file settingsmanager.h
 * @brief Definition of the SettingsManager class
 * 
 * This file defines the SettingsManager singleton class which manages application settings
 * including window appearance, startup behavior, and notification preferences.
 * 
 * @author Cornebidouil
 * @date Last updated: April 28, 2025
 */

#pragma once

#include <QObject>
#include <QSettings>
#include <QPoint>
#include <QSize>
#include <QDebug>

/**
 * @class SettingsManager
 * @brief Singleton manager for application settings
 * 
 * The SettingsManager class provides a centralized interface for accessing and
 * modifying application settings. It is implemented as a singleton to ensure
 * that there is only one settings manager throughout the application.
 * 
 * It handles:
 * - Window appearance settings (position, size, opacity, always-on-top)
 * - Startup behavior settings (auto-start, minimized start)
 * - Category defaults
 * - Notification preferences
 */
class SettingsManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     * 
     * Returns the single instance of the SettingsManager class,
     * creating it if it doesn't already exist.
     * 
     * @return SettingsManager& Reference to the singleton instance
     */
    static SettingsManager& instance();

    // Window settings
    /**
     * @brief Check if window should always be on top
     * @return bool True if window should always be on top
     */
    bool alwaysOnTop() const;
    
    /**
     * @brief Set the always-on-top state
     * @param alwaysOnTop New always-on-top state
     */
    void setAlwaysOnTop(bool alwaysOnTop);

    /**
     * @brief Get window opacity
     * @return double Window opacity (0.0 to 1.0)
     */
    double opacity() const;
    
    /**
     * @brief Set window opacity
     * @param opacity New opacity value (0.0 to 1.0)
     */
    void setOpacity(double opacity);

    /**
     * @brief Get window position
     * @return QPoint Last saved window position
     */
    QPoint windowPosition() const;
    
    /**
     * @brief Set window position
     * @param position New window position
     */
    void setWindowPosition(const QPoint &position);

    /**
     * @brief Get window size
     * @return QSize Last saved window size
     */
    QSize windowSize() const;
    
    /**
     * @brief Set window size
     * @param size New window size
     */
    void setWindowSize(const QSize &size);

    // Startup settings
    /**
     * @brief Check if app should start with Windows
     * @return bool True if app should auto-start with Windows
     */
    bool startWithWindows() const;
    
    /**
     * @brief Set auto-start with Windows
     * @param start Whether app should auto-start with Windows
     */
    void setStartWithWindows(bool start);

    /**
     * @brief Check if app should start minimized
     * @return bool True if app should start minimized
     */
    bool startMinimized() const;
    
    /**
     * @brief Set start minimized
     * @param minimized Whether app should start minimized
     */
    void setStartMinimized(bool minimized);

    // Category settings
    /**
     * @brief Get default category ID
     * @return QString ID of the default category
     */
    QString defaultCategoryId() const;
    
    /**
     * @brief Set default category ID
     * @param categoryId ID of the new default category
     */
    void setDefaultCategoryId(const QString &categoryId);

    // Notification settings
    /**
     * @brief Check if notifications are enabled
     * @return bool True if notifications are enabled
     */
    bool enableNotifications() const;
    
    /**
     * @brief Set notifications enabled state
     * @param enable Whether notifications should be enabled
     */
    void setEnableNotifications(bool enable);

    /**
     * @brief Check if due soon notifications are enabled
     * @return bool True if due soon notifications are enabled
     */
    bool notifyDueSoon() const;
    
    /**
     * @brief Set due soon notifications state
     * @param notify Whether due soon notifications should be enabled
     */
    void setNotifyDueSoon(bool notify);

    /**
     * @brief Check if overdue notifications are enabled
     * @return bool True if overdue notifications are enabled
     */
    bool notifyOverdue() const;
    
    /**
     * @brief Set overdue notifications state
     * @param notify Whether overdue notifications should be enabled
     */
    void setNotifyOverdue(bool notify);

    /**
     * @brief Load settings from storage
     * 
     * Loads all settings from persistent storage into memory.
     */
    void load();
    
    /**
     * @brief Save settings to storage
     * 
     * Saves all current settings to persistent storage.
     */
    void save();
    
    /**
     * @brief Reset all settings to defaults
     * 
     * Resets all settings to their default values.
     */
    void resetToDefaults();

private:
    /**
     * @brief Private constructor
     * 
     * Prevents direct instantiation to ensure singleton pattern.
     * 
     * @param parent Optional parent QObject
     */
    SettingsManager(QObject* parent = nullptr);
    
    /**
     * @brief Private destructor
     * 
     * Ensures settings are saved when the manager is destroyed.
     */
    ~SettingsManager();

    QSettings m_settings;      ///< Qt settings storage

    // Cached settings
    bool m_alwaysOnTop;        ///< Whether window should be always on top
    double m_opacity;          ///< Window opacity (0.0 to 1.0)
    QPoint m_windowPosition;   ///< Window position
    QSize m_windowSize;        ///< Window size
    bool m_startWithWindows;   ///< Whether app should auto-start with Windows
    bool m_startMinimized;     ///< Whether app should start minimized
    QString m_defaultCategoryId; ///< Default category ID
    bool m_enableNotifications; ///< Whether notifications are enabled
    bool m_notifyDueSoon;      ///< Whether due soon notifications are enabled
    bool m_notifyOverdue;      ///< Whether overdue notifications are enabled
};
