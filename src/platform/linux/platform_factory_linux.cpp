#include "../platform_interface.h"
#include "keyboard_hook_linux.h"
#include "text_input_linux.h"

namespace UniLang {
namespace Platform {

IKeyboardHook* PlatformFactory::CreateKeyboardHook() {
    return new KeyboardHookLinux();
}

IWindow* PlatformFactory::CreateWindow() {
    // TODO: Implement Linux window
    return nullptr;
}

ITextInput* PlatformFactory::CreateTextInput() {
    return new TextInputLinux();
}

ISystemTray* PlatformFactory::CreateSystemTray() {
    // TODO: Implement Linux system tray
    return nullptr;
}

} // namespace Platform
} // namespace UniLang
