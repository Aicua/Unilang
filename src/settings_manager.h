#pragma once

#include <string>
#include <Windows.h>

namespace UniLang {

/**
 * @brief Manages application settings and system tray icon
 *
 * Responsibilities:
 * - Create system tray icon
 * - Show context menu (Enable/Disable, Settings, Exit)
 * - Load/save settings from JSON
 */
class SettingsManager {
public:
    struct Settings {
        bool enabled = true;
        bool show_popup = false;  // Popup disabled by default
        int popup_duration_ms = 1000;
        std::string trigger_key = "\\";
        bool case_sensitive = true;
    };

    SettingsManager();
    ~SettingsManager();

    /**
     * @brief Initialize system tray icon
     * @param hwnd Main window handle for messages
     * @return true if successful
     */
    bool InitializeTray(HWND hwnd);

    /**
     * @brief Remove tray icon
     */
    void RemoveTray();

    /**
     * @brief Load settings from JSON file
     */
    bool LoadSettings(const std::string& filepath);

    /**
     * @brief Save settings to JSON file
     */
    bool SaveSettings(const std::string& filepath);

    /**
     * @brief Get current settings
     */
    const Settings& GetSettings() const { return m_settings; }

    /**
     * @brief Toggle enabled state
     */
    void ToggleEnabled();

    /**
     * @brief Show tray context menu
     */
    void ShowContextMenu(HWND hwnd, POINT pt);

    /**
     * @brief Handle tray icon click
     */
    void OnTrayIconClick(UINT msg);

private:
    /**
     * @brief Update tray icon tooltip
     */
    void UpdateTrayTooltip();

private:
    Settings m_settings;
    NOTIFYICONDATA m_nid = {};
    bool m_tray_initialized = false;

    // Menu item IDs
    static const UINT ID_TRAY_ENABLE = 1001;
    static const UINT ID_TRAY_SETTINGS = 1002;
    static const UINT ID_TRAY_EXIT = 1003;

    static const UINT WM_TRAYICON = WM_USER + 1;
};

} // namespace UniLang
