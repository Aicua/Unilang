#pragma once

#include "../platform_interface.h"

namespace UniLang {
namespace Platform {

class TextInputLinux : public ITextInput {
public:
    TextInputLinux() = default;
    ~TextInputLinux() override = default;

    void SendBackspace(int count) override;
    void SendText(const std::string& text) override;
    void SendKey(VirtualKey key, bool shift = false) override;
};

} // namespace Platform
} // namespace UniLang
