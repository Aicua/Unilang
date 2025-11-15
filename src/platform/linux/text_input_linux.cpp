#include "text_input_linux.h"
#include <iostream>

namespace UniLang {
namespace Platform {

void TextInputLinux::SendBackspace(int count) {
    // TODO: Implement using X11 XTestFakeKeyEvent or similar
    std::cerr << "Warning: SendBackspace not yet implemented on Linux" << std::endl;
}

void TextInputLinux::SendText(const std::string& text) {
    // TODO: Implement using X11 or Wayland text input
    std::cerr << "Warning: SendText not yet implemented on Linux" << std::endl;
}

void TextInputLinux::SendKey(VirtualKey key, bool shift) {
    // TODO: Implement using X11 XTestFakeKeyEvent or similar
    std::cerr << "Warning: SendKey not yet implemented on Linux" << std::endl;
}

} // namespace Platform
} // namespace UniLang
