#include "keyboard_hook.h"
#include <iostream>

namespace UniLang {

// Static instance pointer for callback
KeyboardHook* KeyboardHook::s_instance = nullptr;

KeyboardHook::KeyboardHook() {
    s_instance = this;
}

KeyboardHook::~KeyboardHook() {
    Uninstall();
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool KeyboardHook::Install(KeyCallback callback) {
    if (m_hook != nullptr) {
//         std::cerr << "Keyboard hook already installed!" << std::endl;
        return false;
    }

    m_callback = callback;

    // Install low-level keyboard hook
    m_hook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        GetModuleHandle(nullptr),
        0
    );

    if (m_hook == nullptr) {
        DWORD error = GetLastError();
//         std::cerr << "Failed to install keyboard hook. Error: " << error << std::endl;
        return false;
    }

//     std::cout << "Keyboard hook installed successfully!" << std::endl;
    return true;
}

void KeyboardHook::Uninstall() {
    if (m_hook != nullptr) {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
//         std::cout << "Keyboard hook uninstalled" << std::endl;
    }
}

LRESULT CALLBACK KeyboardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (s_instance != nullptr) {
        return s_instance->HandleKeyEvent(nCode, wParam, lParam);
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT KeyboardHook::HandleKeyEvent(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

        // Call the registered callback
        if (m_callback) {
            bool shouldBlock = m_callback(kb->vkCode, isKeyDown);

            // If callback returns true, block the key event
            if (shouldBlock) {
                return 1; // Block the key
            }
        }
    }

    // Pass to next hook
    return CallNextHookEx(m_hook, nCode, wParam, lParam);
}

} // namespace UniLang
