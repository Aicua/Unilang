#include "text_replacer.h"
#include <vector>
#include <iostream>

namespace UniLang {

bool TextReplacer::Replace(size_t pattern_length, const std::string& replacement) {
    // Step 1: Send backspaces to delete the pattern
    SendBackspaces(pattern_length);

    // Delay to ensure backspaces are processed before sending Unicode
    // Increased for VSCode/Claude CLI compatibility (50ms is critical)
    Sleep(50);

    // Step 2: Send the Unicode replacement
    SendUnicodeText(replacement);

    return true;
}

void TextReplacer::SendBackspaces(size_t count) {
    std::vector<INPUT> inputs;
    inputs.reserve(count * 2); // Each key press needs key down + key up

    for (size_t i = 0; i < count; ++i) {
        // Key down
        INPUT input_down = {};
        input_down.type = INPUT_KEYBOARD;
        input_down.ki.wVk = VK_BACK;
        input_down.ki.dwFlags = 0;
        inputs.push_back(input_down);

        // Key up
        INPUT input_up = {};
        input_up.type = INPUT_KEYBOARD;
        input_up.ki.wVk = VK_BACK;
        input_up.ki.dwFlags = KEYEVENTF_KEYUP;
        inputs.push_back(input_up);
    }

    if (!inputs.empty()) {
        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }
}

void TextReplacer::SendUnicodeText(const std::string& text) {
    // Convert UTF-8 to UTF-16
    std::wstring wtext = Utf8ToUtf16(text);

    // Send each character
    for (wchar_t ch : wtext) {
        SendUnicodeChar(ch);
    }
}

std::wstring TextReplacer::Utf8ToUtf16(const std::string& utf8) {
    if (utf8.empty()) {
        return std::wstring();
    }

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(),
                                          static_cast<int>(utf8.size()),
                                          nullptr, 0);
    if (size_needed <= 0) {
        return std::wstring();
    }

    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(),
                       static_cast<int>(utf8.size()),
                       &result[0], size_needed);

    return result;
}

void TextReplacer::SendUnicodeChar(wchar_t ch) {
    INPUT inputs[2] = {};

    // Key down
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = 0;
    inputs[0].ki.wScan = ch;
    inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;

    // Key up
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0;
    inputs[1].ki.wScan = ch;
    inputs[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

} // namespace UniLang
