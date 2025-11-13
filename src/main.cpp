#include <Windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>

#include "keyboard_hook.h"
#include "shortcuts_dict.h"
#include "pattern_matcher.h"
#include "text_replacer.h"
#include "popup_window.h"
#include "settings_manager.h"
#include "help_window.h"

namespace fs = std::filesystem;

// Global application state
struct AppState {
    UniLang::KeyboardHook keyboard_hook;
    UniLang::ShortcutsDict shortcuts_dict;
    UniLang::PatternMatcher pattern_matcher;
    UniLang::TextReplacer text_replacer;
    UniLang::PopupWindow popup_window;
    UniLang::SettingsManager settings_manager;
    UniLang::HelpWindow help_window;

    HWND main_window = nullptr;
    bool running = true;
};

AppState* g_app = nullptr;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool OnKeyEvent(DWORD vkCode, bool isKeyDown);
std::string GetExecutableDir();
bool IsVSCodeWindow();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // Check for single instance (prevent multiple instances running)
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, L"UniLang_SingleInstance_Mutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // Another instance is already running
        MessageBoxW(nullptr,
                   L"UniLang is already running!\n\nCheck the system tray for the UniLang icon.",
                   L"UniLang - Already Running",
                   MB_OK | MB_ICONINFORMATION);
        if (hMutex) {
            CloseHandle(hMutex);
        }
        return 0;
    }

    // Note: Console logging is disabled for release build
    // To enable debug console, uncomment the following lines:
    /*
    AllocConsole();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    std::cout << "========================================" << std::endl;
    std::cout << "       UniLang - Universal Input       " << std::endl;
    std::cout << "   LaTeX-style Math & Symbol Shortcuts  " << std::endl;
    std::cout << "========================================" << std::endl;
    */

    // Create application state
    AppState app;
    g_app = &app;

    // Load shortcuts dictionary from embedded resource
    if (!app.shortcuts_dict.LoadFromResource()) {
        MessageBoxA(nullptr,
                   "Failed to load shortcuts from resource!",
                   "UniLang - Error",
                   MB_OK | MB_ICONERROR);
        return 1;
    }

    // Note: Settings use default values (enabled=true, show_popup=true, etc.)
    // No external config file needed

    // Create invisible main window for message loop
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"UniLangMainWindow";

    if (!RegisterClassExW(&wc)) {
        MessageBoxA(nullptr, "Failed to register window class!", "UniLang - Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    app.main_window = CreateWindowExW(
        0,
        L"UniLangMainWindow",
        L"UniLang",
        WS_OVERLAPPEDWINDOW,
        0, 0, 0, 0,
        nullptr, nullptr, hInstance, nullptr
    );

    if (app.main_window == nullptr) {
        MessageBoxA(nullptr, "Failed to create main window!", "UniLang - Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Initialize system tray
    if (!app.settings_manager.InitializeTray(app.main_window)) {
        MessageBoxA(nullptr, "Failed to create system tray icon!", "UniLang - Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Create popup window
    if (!app.popup_window.Create(hInstance)) {
        MessageBoxA(nullptr, "Failed to create popup window!", "UniLang - Warning", MB_OK | MB_ICONWARNING);
        // Continue anyway - popup is optional
    }

    // Create help window
    if (!app.help_window.Create(hInstance, &app.shortcuts_dict)) {
        MessageBoxA(nullptr, "Failed to create help window!", "UniLang - Warning", MB_OK | MB_ICONWARNING);
        // Continue anyway - help is optional
    }

    // Set callback for tray icon left-click to show help window
    app.settings_manager.SetOnHelpRequestCallback([&app]() {
        app.help_window.Toggle();
    });

    // Install keyboard hook
    if (!app.keyboard_hook.Install(OnKeyEvent)) {
        MessageBoxA(nullptr, "Failed to install keyboard hook!", "UniLang - Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // UniLang is now running silently in the background
    // Check the system tray icon for status and options

    // Message loop
    MSG msg;
    while (app.running && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    app.keyboard_hook.Uninstall();
    app.settings_manager.RemoveTray();

    return 0;
}

bool OnKeyEvent(DWORD vkCode, bool isKeyDown) {
    if (!g_app || !isKeyDown) {
        return false; // Don't block key-up events
    }

    // Check if UniLang is enabled
    if (!g_app->settings_manager.GetSettings().enabled) {
        return false;
    }

    // Handle special keys that should reset the pattern buffer
    // NOTE: VK_SPACE is NOT here because it's used as trigger for LaTeX patterns
    if (vkCode == VK_RETURN || vkCode == VK_ESCAPE || vkCode == VK_TAB) {
        g_app->pattern_matcher.Reset();
        return false;
    }

    // Handle Backspace - remove last character from buffer
    if (vkCode == VK_BACK) {
        g_app->pattern_matcher.RemoveLastChar();
        return false; // Don't block backspace
    }

    // Convert virtual key to character
    BYTE keyState[256] = {};
    GetKeyboardState(keyState);

    // Manually check for modifier keys using GetAsyncKeyState
    // This is more reliable in keyboard hook context
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        keyState[VK_SHIFT] = 0x80;
    }
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
        keyState[VK_CONTROL] = 0x80;
    }
    if (GetAsyncKeyState(VK_MENU) & 0x8000) {  // Alt key
        keyState[VK_MENU] = 0x80;
    }

    char buffer[2] = {};
    int result = ToAscii(vkCode, MapVirtualKey(vkCode, MAPVK_VK_TO_VSC),
                        keyState, reinterpret_cast<LPWORD>(buffer), 0);

    if (result == 1) {
        char ch = buffer[0];

        // Add character to pattern matcher
        auto match = g_app->pattern_matcher.AddChar(ch);

        if (match) {
            // Found a pattern! Check if we have a replacement
            auto replacement = g_app->shortcuts_dict.FindReplacement(match->pattern);

            if (replacement) {

                // Show popup preview
                if (g_app->settings_manager.GetSettings().show_popup) {
                    g_app->popup_window.Show(
                        match->pattern,
                        *replacement,
                        g_app->settings_manager.GetSettings().popup_duration_ms
                    );
                }

                // Determine if this is a LaTeX pattern (starts with backslash)
                bool isLatexPattern = (match->pattern.length() > 0 && match->pattern[0] == '\\');

                // Always block the trigger key and use delays to handle timing
                Sleep(50);

                // Replace text
                // For length == 1, we're in auto-convert mode (superscript/subscript)
                // Just replace the current character without backspace
                if (match->length == 1) {
                    g_app->text_replacer.Replace(0, *replacement);
                } else {
                    // Use (length - 1) because trigger key is blocked
                    // match->length includes the trigger key which hasn't been sent yet
                    g_app->text_replacer.Replace(match->length - 1, *replacement);
                }

                // Don't reset pattern matcher if we're still in mode
                if (!g_app->pattern_matcher.IsInSuperscriptMode() &&
                    !g_app->pattern_matcher.IsInSubscriptMode()) {
                    g_app->pattern_matcher.Reset();
                }

                // ALWAYS block the trigger key
                return true;
            }
            // Pattern detected but no replacement found - silently ignore
        }
    }

    return false; // Don't block normal typing
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    const UINT WM_TRAYICON = WM_USER + 1;

    switch (msg) {
        case WM_CREATE:
            return 0;

        case WM_DESTROY:
            if (g_app) {
                g_app->running = false;
            }
            PostQuitMessage(0);
            return 0;

        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                // Show context menu
                POINT pt;
                GetCursorPos(&pt);
                if (g_app) {
                    g_app->settings_manager.ShowContextMenu(hwnd, pt);
                }
            } else if (lParam == WM_LBUTTONDBLCLK) {
                // Toggle enabled state
                if (g_app) {
                    g_app->settings_manager.ToggleEnabled();
                }
            }
            return 0;

        case WM_COMMAND:
            if (g_app) {
                switch (LOWORD(wParam)) {
                    case 1001: // ID_TRAY_ENABLE
                        g_app->settings_manager.ToggleEnabled();
                        break;

                    case 1002: // ID_TRAY_SETTINGS
                        MessageBoxW(hwnd,
                                   L"Settings dialog coming soon!\n\nFor now, edit shortcuts.json manually.",
                                   L"UniLang Settings",
                                   MB_OK | MB_ICONINFORMATION);
                        break;

                    case 1003: // ID_TRAY_EXIT
                        DestroyWindow(hwnd);
                        break;
                }
            }
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

std::string GetExecutableDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);

    fs::path exe_path(path);
    return exe_path.parent_path().string();
}

bool IsVSCodeWindow() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return false;
    }

    // Get window title
    wchar_t title[512] = {};
    GetWindowTextW(foreground, title, 512);
    std::wstring wtitle(title);

    // Get window class name
    wchar_t className[256] = {};
    GetClassNameW(foreground, className, 256);
    std::wstring wclassName(className);

    // Get process name
    DWORD processId = 0;
    GetWindowThreadProcessId(foreground, &processId);

    wchar_t processName[MAX_PATH] = {};
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (hProcess) {
        DWORD size = MAX_PATH;
        QueryFullProcessImageNameW(hProcess, 0, processName, &size);
        CloseHandle(hProcess);
    }
    std::wstring wprocessName(processName);

    // Convert to lowercase for case-insensitive comparison
    std::transform(wtitle.begin(), wtitle.end(), wtitle.begin(), ::tolower);
    std::transform(wclassName.begin(), wclassName.end(), wclassName.begin(), ::tolower);
    std::transform(wprocessName.begin(), wprocessName.end(), wprocessName.begin(), ::tolower);

    // Check if it's VSCode
    // - Title contains "visual studio code"
    // - Process name contains "code.exe" or "code - insiders.exe"
    // - Window class is Chrome-based (VSCode uses Electron)
    if (wtitle.find(L"visual studio code") != std::wstring::npos ||
        wprocessName.find(L"code.exe") != std::wstring::npos ||
        wprocessName.find(L"code - insiders.exe") != std::wstring::npos ||
        (wclassName.find(L"chrome") != std::wstring::npos &&
         wtitle.find(L"code") != std::wstring::npos)) {
        return true;
    }

    return false;
}
