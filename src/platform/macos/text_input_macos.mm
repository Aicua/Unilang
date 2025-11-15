#include "text_input_macos.h"
#include <CoreGraphics/CoreGraphics.h>
#include <Carbon/Carbon.h>
#include <iostream>

namespace UniLang {
namespace Platform {

void TextInputMacOS::SendBackspace(int count) {
    for (int i = 0; i < count; ++i) {
        SendKeyEvent(kVK_Delete, false);
    }
}

void TextInputMacOS::SendText(const std::string& text) {
    // Convert UTF-8 string to Unicode characters and send them
    // Note: This is a simplified implementation
    // For full Unicode support, we need to handle multi-byte characters properly

    for (size_t i = 0; i < text.length(); ++i) {
        unsigned char c = text[i];

        // Handle ASCII characters
        if (c < 128) {
            // Create a keyboard event with the Unicode character
            CGEventRef keyDownEvent = CGEventCreateKeyboardEvent(nullptr, 0, true);
            CGEventRef keyUpEvent = CGEventCreateKeyboardEvent(nullptr, 0, false);

            if (keyDownEvent && keyUpEvent) {
                // Set the Unicode string
                UniChar unicodeChar = static_cast<UniChar>(c);
                CGEventKeyboardSetUnicodeString(keyDownEvent, 1, &unicodeChar);
                CGEventKeyboardSetUnicodeString(keyUpEvent, 1, &unicodeChar);

                // Post the events
                CGEventPost(kCGHIDEventTap, keyDownEvent);
                CGEventPost(kCGHIDEventTap, keyUpEvent);

                CFRelease(keyDownEvent);
                CFRelease(keyUpEvent);
            }
        } else {
            // For multi-byte UTF-8 characters, we need special handling
            // This is a simplified version - full implementation would decode UTF-8
            std::cerr << "Warning: Multi-byte character support not fully implemented" << std::endl;
        }

        // Small delay between characters
        usleep(1000); // 1ms
    }
}

void TextInputMacOS::SendKey(VirtualKey key, bool shift) {
    CGKeyCode keyCode = VirtualKeyToCGKeyCode(key);
    SendKeyEvent(keyCode, shift);
}

void TextInputMacOS::SendKeyEvent(CGKeyCode keyCode, bool shift) {
    CGEventRef keyDown;
    CGEventRef keyUp;

    if (shift) {
        // Press shift
        CGEventRef shiftDown = CGEventCreateKeyboardEvent(nullptr, kVK_Shift, true);
        CGEventPost(kCGHIDEventTap, shiftDown);
        CFRelease(shiftDown);
    }

    // Press key
    keyDown = CGEventCreateKeyboardEvent(nullptr, keyCode, true);
    CGEventPost(kCGHIDEventTap, keyDown);
    CFRelease(keyDown);

    // Small delay
    usleep(5000); // 5ms

    // Release key
    keyUp = CGEventCreateKeyboardEvent(nullptr, keyCode, false);
    CGEventPost(kCGHIDEventTap, keyUp);
    CFRelease(keyUp);

    if (shift) {
        // Release shift
        CGEventRef shiftUp = CGEventCreateKeyboardEvent(nullptr, kVK_Shift, false);
        CGEventPost(kCGHIDEventTap, shiftUp);
        CFRelease(shiftUp);
    }
}

CGKeyCode TextInputMacOS::VirtualKeyToCGKeyCode(VirtualKey key) {
    // Map VirtualKey enum to macOS key codes
    switch (key) {
        case VirtualKey::Backspace:   return kVK_Delete;
        case VirtualKey::Tab:         return kVK_Tab;
        case VirtualKey::Enter:       return kVK_Return;
        case VirtualKey::Shift:       return kVK_Shift;
        case VirtualKey::Control:     return kVK_Control;
        case VirtualKey::Alt:         return kVK_Option;
        case VirtualKey::Escape:      return kVK_Escape;
        case VirtualKey::Space:       return kVK_Space;
        case VirtualKey::Left:        return kVK_LeftArrow;
        case VirtualKey::Up:          return kVK_UpArrow;
        case VirtualKey::Right:       return kVK_RightArrow;
        case VirtualKey::Down:        return kVK_DownArrow;
        case VirtualKey::Delete:      return kVK_ForwardDelete;

        // Letters A-Z
        case VirtualKey::A: return kVK_ANSI_A;
        case VirtualKey::B: return kVK_ANSI_B;
        case VirtualKey::C: return kVK_ANSI_C;
        case VirtualKey::D: return kVK_ANSI_D;
        case VirtualKey::E: return kVK_ANSI_E;
        case VirtualKey::F: return kVK_ANSI_F;
        case VirtualKey::G: return kVK_ANSI_G;
        case VirtualKey::H: return kVK_ANSI_H;
        case VirtualKey::I: return kVK_ANSI_I;
        case VirtualKey::J: return kVK_ANSI_J;
        case VirtualKey::K: return kVK_ANSI_K;
        case VirtualKey::L: return kVK_ANSI_L;
        case VirtualKey::M: return kVK_ANSI_M;
        case VirtualKey::N: return kVK_ANSI_N;
        case VirtualKey::O: return kVK_ANSI_O;
        case VirtualKey::P: return kVK_ANSI_P;
        case VirtualKey::Q: return kVK_ANSI_Q;
        case VirtualKey::R: return kVK_ANSI_R;
        case VirtualKey::S: return kVK_ANSI_S;
        case VirtualKey::T: return kVK_ANSI_T;
        case VirtualKey::U: return kVK_ANSI_U;
        case VirtualKey::V: return kVK_ANSI_V;
        case VirtualKey::W: return kVK_ANSI_W;
        case VirtualKey::X: return kVK_ANSI_X;
        case VirtualKey::Y: return kVK_ANSI_Y;
        case VirtualKey::Z: return kVK_ANSI_Z;

        // Numbers 0-9
        case VirtualKey::Num0: return kVK_ANSI_0;
        case VirtualKey::Num1: return kVK_ANSI_1;
        case VirtualKey::Num2: return kVK_ANSI_2;
        case VirtualKey::Num3: return kVK_ANSI_3;
        case VirtualKey::Num4: return kVK_ANSI_4;
        case VirtualKey::Num5: return kVK_ANSI_5;
        case VirtualKey::Num6: return kVK_ANSI_6;
        case VirtualKey::Num7: return kVK_ANSI_7;
        case VirtualKey::Num8: return kVK_ANSI_8;
        case VirtualKey::Num9: return kVK_ANSI_9;

        default: return 0;
    }
}

} // namespace Platform
} // namespace UniLang
