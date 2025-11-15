#pragma once

#include "../platform_interface.h"

namespace UniLang {
namespace Platform {

class KeyboardHookLinux : public IKeyboardHook {
public:
    KeyboardHookLinux();
    ~KeyboardHookLinux() override;

    bool Install(KeyCallback callback) override;
    void Uninstall() override;
    bool IsInstalled() const override { return m_installed; }

private:
    bool m_installed = false;
    KeyCallback m_callback;
};

} // namespace Platform
} // namespace UniLang
