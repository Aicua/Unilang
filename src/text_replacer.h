#pragma once

#include <string>
#include <Windows.h>

namespace UniLang {

/**
 * @brief Handles text replacement by sending backspaces and Unicode input
 *
 * Uses Windows SendInput API to:
 * 1. Delete the shortcut pattern (e.g., send 6 backspaces for "\alpha")
 * 2. Insert Unicode character (e.g., "α")
 */
class TextReplacer {
public:
    TextReplacer() = default;
    ~TextReplacer() = default;

    /**
     * @brief Replace text at current cursor position
     * @param pattern_length Number of characters to delete (backspace)
     * @param replacement Unicode string to insert
     * @return true if successful
     */
    bool Replace(size_t pattern_length, const std::string& replacement);

    /**
     * @brief Send backspace key presses
     * @param count Number of backspaces to send
     */
    void SendBackspaces(size_t count);

    /**
     * @brief Send Unicode text
     * @param text UTF-8 encoded text to send
     */
    void SendUnicodeText(const std::string& text);

private:
    /**
     * @brief Convert UTF-8 string to UTF-16 (for Windows API)
     */
    std::wstring Utf8ToUtf16(const std::string& utf8);

    /**
     * @brief Send a single Unicode character using SendInput
     */
    void SendUnicodeChar(wchar_t ch);
};

} // namespace UniLang
