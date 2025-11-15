#pragma once

#include "../platform_interface.h"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>

namespace UniLang {
namespace Platform {

/**
 * @brief macOS keyboard hook implementation using CGEventTap
 */
class KeyboardHookMacOS : public IKeyboardHook {
public:
    KeyboardHookMacOS();
    ~KeyboardHookMacOS() override;

    bool Install(KeyCallback callback) override;
    void Uninstall() override;
    bool IsInstalled() const override { return m_installed; }

private:
    /**
     * @brief Static callback for CGEventTap
     */
    static CGEventRef EventTapCallback(
        CGEventTapProxy proxy,
        CGEventType type,
        CGEventRef event,
        void* userInfo
    );

    /**
     * @brief Instance method called from static callback
     */
    CGEventRef HandleEvent(CGEventType type, CGEventRef event);

private:
    bool m_installed = false;
    KeyCallback m_callback;
    CFMachPortRef m_eventTap = nullptr;
    CFRunLoopSourceRef m_runLoopSource = nullptr;

    static KeyboardHookMacOS* s_instance;
};

} // namespace Platform
} // namespace UniLang
