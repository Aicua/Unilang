#pragma once

#include <string>
#include <functional>
#include "platform_types.h"

namespace UniLang {
namespace Platform {

/**
 * @brief Interface for platform-specific keyboard hook
 */
class IKeyboardHook {
public:
    using KeyCallback = std::function<bool(uint32_t vkCode, bool isKeyDown)>;

    virtual ~IKeyboardHook() = default;

    virtual bool Install(KeyCallback callback) = 0;
    virtual void Uninstall() = 0;
    virtual bool IsInstalled() const = 0;
};

/**
 * @brief Interface for platform-specific window management
 */
class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool Create(const char* title, int x, int y, int width, int height) = 0;
    virtual void Destroy() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void SetPosition(int x, int y) = 0;
    virtual void SetSize(int width, int height) = 0;
    virtual PlatformWindow GetNativeHandle() const = 0;
};

/**
 * @brief Interface for platform-specific text input simulation
 */
class ITextInput {
public:
    virtual ~ITextInput() = default;

    virtual void SendBackspace(int count) = 0;
    virtual void SendText(const std::string& text) = 0;
    virtual void SendKey(VirtualKey key, bool shift = false) = 0;
};

/**
 * @brief Interface for platform-specific system tray
 */
class ISystemTray {
public:
    using MenuCallback = std::function<void(int menuId)>;

    virtual ~ISystemTray() = default;

    virtual bool Create(PlatformWindow window, const char* tooltip) = 0;
    virtual void Destroy() = 0;
    virtual void SetTooltip(const char* tooltip) = 0;
    virtual void ShowNotification(const char* title, const char* message) = 0;
    virtual void SetMenuCallback(MenuCallback callback) = 0;
};

/**
 * @brief Factory for creating platform-specific implementations
 */
class PlatformFactory {
public:
    static IKeyboardHook* CreateKeyboardHook();
    static IWindow* CreateWindow();
    static ITextInput* CreateTextInput();
    static ISystemTray* CreateSystemTray();
};

} // namespace Platform
} // namespace UniLang
