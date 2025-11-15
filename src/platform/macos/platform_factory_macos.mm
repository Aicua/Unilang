#include "../platform_interface.h"
#include "keyboard_hook_macos.h"
#include "text_input_macos.h"

namespace UniLang {
namespace Platform {

IKeyboardHook* PlatformFactory::CreateKeyboardHook() {
    return new KeyboardHookMacOS();
}

IWindow* PlatformFactory::CreateWindow() {
    // TODO: Implement macOS window (NSWindow)
    return nullptr;
}

ITextInput* PlatformFactory::CreateTextInput() {
    return new TextInputMacOS();
}

ISystemTray* PlatformFactory::CreateSystemTray() {
    // TODO: Implement macOS system tray (NSStatusItem)
    return nullptr;
}

} // namespace Platform
} // namespace UniLang
