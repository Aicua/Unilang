#include "settings_manager.h"
#include "resource.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace UniLang {

SettingsManager::SettingsManager() {
}

SettingsManager::~SettingsManager() {
    RemoveTray();
}

bool SettingsManager::InitializeTray(HWND hwnd) {
    // Initialize NOTIFYICONDATA structure
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;

    // Load custom UniLang icon from resources
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    m_nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    // Fallback to default icon if custom icon fails to load
    if (m_nid.hIcon == nullptr) {
        m_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    }

    // Set tooltip
    strcpy_s(m_nid.szTip, sizeof(m_nid.szTip), "UniLang - Math & Symbol Input");

    // Add tray icon
    if (!Shell_NotifyIcon(NIM_ADD, &m_nid)) {
        DWORD error = GetLastError();
//         std::cerr << "Failed to create tray icon. Error: " << error << std::endl;
        return false;
    }

    m_tray_initialized = true;
//     std::cout << "System tray icon created successfully!" << std::endl;
    return true;
}

void SettingsManager::RemoveTray() {
    if (m_tray_initialized) {
        Shell_NotifyIcon(NIM_DELETE, &m_nid);
        m_tray_initialized = false;
    }
}

bool SettingsManager::LoadSettings(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
//             std::cerr << "Failed to open settings file: " << filepath << std::endl;
            return false;
        }

        json j;
        file >> j;

        if (j.contains("settings")) {
            auto settings = j["settings"];

            if (settings.contains("enabled")) {
                m_settings.enabled = settings["enabled"];
            }
            if (settings.contains("show_popup")) {
                m_settings.show_popup = settings["show_popup"];
            }
            if (settings.contains("popup_duration_ms")) {
                m_settings.popup_duration_ms = settings["popup_duration_ms"];
            }
            if (settings.contains("trigger_key")) {
                m_settings.trigger_key = settings["trigger_key"];
            }
            if (settings.contains("case_sensitive")) {
                m_settings.case_sensitive = settings["case_sensitive"];
            }
        }

//         std::cout << "Settings loaded successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
//         std::cerr << "Error loading settings: " << e.what() << std::endl;
        return false;
    }
}

bool SettingsManager::SaveSettings(const std::string& filepath) {
    try {
        // Read existing file to preserve shortcuts
        json j;
        std::ifstream file_in(filepath);
        if (file_in.is_open()) {
            file_in >> j;
            file_in.close();
        }

        // Update settings section
        j["settings"]["enabled"] = m_settings.enabled;
        j["settings"]["show_popup"] = m_settings.show_popup;
        j["settings"]["popup_duration_ms"] = m_settings.popup_duration_ms;
        j["settings"]["trigger_key"] = m_settings.trigger_key;
        j["settings"]["case_sensitive"] = m_settings.case_sensitive;

        // Write back to file
        std::ofstream file_out(filepath);
        if (!file_out.is_open()) {
//             std::cerr << "Failed to open settings file for writing: " << filepath << std::endl;
            return false;
        }

        file_out << j.dump(2) << std::endl;
//         std::cout << "Settings saved successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
//         std::cerr << "Error saving settings: " << e.what() << std::endl;
        return false;
    }
}

void SettingsManager::ToggleEnabled() {
    m_settings.enabled = !m_settings.enabled;
    UpdateTrayTooltip();

//     std::cout << "UniLang " << (m_settings.enabled ? "ENABLED" : "DISABLED") << std::endl;
}

void SettingsManager::ShowContextMenu(HWND hwnd, POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    if (hMenu == nullptr) {
        return;
    }

    // Add menu items
    // Use MF_CHECKED flag for proper Windows checkmark instead of Unicode
    UINT flags = MF_STRING | (m_settings.enabled ? MF_CHECKED : MF_UNCHECKED);
    AppendMenuW(hMenu, flags, ID_TRAY_ENABLE, L"Enabled");

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_SETTINGS, L"Settings...");
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    // Set foreground window (required for menu to close properly)
    SetForegroundWindow(hwnd);

    // Show menu
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN,
                   pt.x, pt.y, 0, hwnd, nullptr);

    // Cleanup
    DestroyMenu(hMenu);
}

void SettingsManager::OnTrayIconClick(UINT msg) {
    if (msg == WM_LBUTTONDBLCLK) {
        // Double-click: toggle enabled state
        ToggleEnabled();
    }
}

void SettingsManager::UpdateTrayTooltip() {
    if (!m_tray_initialized) {
        return;
    }

    if (m_settings.enabled) {
        strcpy_s(m_nid.szTip, sizeof(m_nid.szTip), "UniLang - ENABLED");
    } else {
        strcpy_s(m_nid.szTip, sizeof(m_nid.szTip), "UniLang - DISABLED");
    }

    Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}

} // namespace UniLang
