#pragma once

#include <Windows.h>
#include <string>

namespace UniLang {

/**
 * @brief Popup tooltip window showing replacement preview
 *
 * Creates a layered window near the cursor that shows:
 * - The shortcut being typed (e.g., "\alpha")
 * - The replacement character (e.g., "α")
 *
 * Auto-hides after timeout or on next keystroke
 */
class PopupWindow {
public:
    PopupWindow();
    ~PopupWindow();

    /**
     * @brief Create the popup window (hidden initially)
     * @param hInstance Application instance handle
     * @return true if created successfully
     */
    bool Create(HINSTANCE hInstance);

    /**
     * @brief Show popup with replacement preview
     * @param shortcut The shortcut pattern (e.g., "\\alpha")
     * @param replacement The replacement text (e.g., "α")
     * @param duration_ms How long to show (milliseconds)
     */
    void Show(const std::string& shortcut, const std::string& replacement, int duration_ms = 1000);

    /**
     * @brief Hide the popup
     */
    void Hide();

    /**
     * @brief Check if popup is currently visible
     */
    bool IsVisible() const { return m_visible; }

private:
    /**
     * @brief Window procedure
     */
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Update window position near cursor
     */
    void UpdatePosition();

    /**
     * @brief Render popup content
     */
    void Render(HDC hdc);

    /**
     * @brief Timer callback to hide popup
     */
    void OnTimer();

private:
    HWND m_hwnd = nullptr;
    HINSTANCE m_hInstance = nullptr;
    bool m_visible = false;

    std::string m_shortcut;
    std::string m_replacement;

    static const UINT TIMER_ID = 1;
    static const wchar_t* WINDOW_CLASS_NAME;
};

} // namespace UniLang
