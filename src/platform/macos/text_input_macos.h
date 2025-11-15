#pragma once

#include "../platform_interface.h"

namespace UniLang {
namespace Platform {

/**
 * @brief macOS text input implementation using CGEventPost
 */
class TextInputMacOS : public ITextInput {
public:
    TextInputMacOS() = default;
    ~TextInputMacOS() override = default;

    void SendBackspace(int count) override;
    void SendText(const std::string& text) override;
    void SendKey(VirtualKey key, bool shift = false) override;

private:
    /**
     * @brief Send a key event (down + up)
     */
    void SendKeyEvent(CGKeyCode keyCode, bool shift = false);

    /**
     * @brief Convert VirtualKey to macOS CGKeyCode
     */
    CGKeyCode VirtualKeyToCGKeyCode(VirtualKey key);
};

} // namespace Platform
} // namespace UniLang
