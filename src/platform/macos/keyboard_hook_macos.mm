#include "keyboard_hook_macos.h"
#include <iostream>

namespace UniLang {
namespace Platform {

KeyboardHookMacOS* KeyboardHookMacOS::s_instance = nullptr;

KeyboardHookMacOS::KeyboardHookMacOS() {
    s_instance = this;
}

KeyboardHookMacOS::~KeyboardHookMacOS() {
    Uninstall();
    s_instance = nullptr;
}

bool KeyboardHookMacOS::Install(KeyCallback callback) {
    if (m_installed) {
        return true;
    }

    m_callback = callback;

    // Create event tap to intercept keyboard events
    CGEventMask eventMask = (1 << kCGEventKeyDown) | (1 << kCGEventKeyUp);

    m_eventTap = CGEventTapCreate(
        kCGSessionEventTap,           // Tap at session level
        kCGHeadInsertEventTap,        // Insert at head of event queue
        kCGEventTapOptionDefault,     // Default options
        eventMask,                     // Event mask
        EventTapCallback,              // Callback function
        this                           // User info
    );

    if (!m_eventTap) {
        std::cerr << "Failed to create event tap. " << std::endl;
        std::cerr << "Please grant Accessibility permissions in System Preferences > Security & Privacy > Privacy > Accessibility" << std::endl;
        return false;
    }

    // Create run loop source and add to current run loop
    m_runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, m_eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), m_runLoopSource, kCFRunLoopCommonModes);

    // Enable the event tap
    CGEventTapEnable(m_eventTap, true);

    m_installed = true;
    std::cout << "macOS keyboard hook installed successfully" << std::endl;
    return true;
}

void KeyboardHookMacOS::Uninstall() {
    if (!m_installed) {
        return;
    }

    if (m_eventTap) {
        CGEventTapEnable(m_eventTap, false);
        CFRelease(m_eventTap);
        m_eventTap = nullptr;
    }

    if (m_runLoopSource) {
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m_runLoopSource, kCFRunLoopCommonModes);
        CFRelease(m_runLoopSource);
        m_runLoopSource = nullptr;
    }

    m_installed = false;
    m_callback = nullptr;
}

CGEventRef KeyboardHookMacOS::EventTapCallback(
    CGEventTapProxy proxy,
    CGEventType type,
    CGEventRef event,
    void* userInfo
) {
    KeyboardHookMacOS* instance = static_cast<KeyboardHookMacOS*>(userInfo);
    if (instance) {
        return instance->HandleEvent(type, event);
    }
    return event;
}

CGEventRef KeyboardHookMacOS::HandleEvent(CGEventType type, CGEventRef event) {
    if (!m_callback) {
        return event;
    }

    // Get key code
    CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

    // Determine if key is down or up
    bool isKeyDown = (type == kCGEventKeyDown);

    // Call the callback
    bool shouldBlock = m_callback(static_cast<uint32_t>(keyCode), isKeyDown);

    // Return null to block the event, or the event to pass it through
    return shouldBlock ? nullptr : event;
}

} // namespace Platform
} // namespace UniLang
