#pragma once

#include <Windows.h>
#include <functional>

namespace UniLang {

/**
 * @brief Low-level keyboard hook for capturing all keystrokes
 *
 * Uses SetWindowsHookEx(WH_KEYBOARD_LL) to intercept keyboard events
 * system-wide. Forwards key events to registered callback.
 */
class KeyboardHook {
public:
    using KeyCallback = std::function<bool(DWORD vkCode, bool isKeyDown)>;

    KeyboardHook();
    ~KeyboardHook();

    /**
     * @brief Install the keyboard hook
     * @param callback Function to call on each key event
     * @return true if hook installed successfully
     */
    bool Install(KeyCallback callback);

    /**
     * @brief Uninstall the keyboard hook
     */
    void Uninstall();

    /**
     * @brief Check if hook is currently installed
     */
    bool IsInstalled() const { return m_hook != nullptr; }

private:
    /**
     * @brief Static callback for Windows hook
     */
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Instance method called from static callback
     */
    LRESULT HandleKeyEvent(int nCode, WPARAM wParam, LPARAM lParam);

private:
    HHOOK m_hook = nullptr;
    KeyCallback m_callback;

    // Singleton instance for static callback
    static KeyboardHook* s_instance;
};

} // namespace UniLang
