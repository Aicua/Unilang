#pragma once

#include <string>
#include <optional>

namespace UniLang {

/**
 * @brief Detects patterns in typed text that should be converted
 *
 * Patterns:
 * - LaTeX-style: \alpha, \beta, etc.
 * - Superscript: x^2, y^a, x^n, etc. (digits and letters)
 * - Subscript: x_1, y_a, x_n, etc. (digits and letters)
 */
class PatternMatcher {
public:
    struct Match {
        std::string pattern;        // The matched pattern (e.g., "\\alpha")
        size_t start_pos;           // Position in buffer where pattern starts
        size_t length;              // Length of pattern
    };

    PatternMatcher();
    ~PatternMatcher() = default;

    /**
     * @brief Add a character to the buffer and check for matches
     * @param ch The character typed by user
     * @return Match if a pattern is detected, nullopt otherwise
     */
    std::optional<Match> AddChar(char ch);

    /**
     * @brief Reset the buffer (e.g., when switching windows)
     */
    void Reset();

    /**
     * @brief Remove last character from buffer (e.g., when user presses Backspace)
     */
    void RemoveLastChar();

    /**
     * @brief Get current buffer content (for debugging)
     */
    const std::string& GetBuffer() const { return m_buffer; }

    /**
     * @brief Check if currently in superscript or subscript mode
     */
    bool IsInSuperscriptMode() const { return m_in_superscript_mode; }
    bool IsInSubscriptMode() const { return m_in_subscript_mode; }

    /**
     * @brief Check if currently in LaTeX mode (typing \word pattern)
     * This is used to block Unikey from interfering with LaTeX patterns
     */
    bool IsInLatexMode() const { return m_in_latex_mode; }

private:
    /**
     * @brief Check if current buffer matches any pattern
     */
    std::optional<Match> CheckForMatch();

    /**
     * @brief Check for LaTeX-style pattern: \word
     */
    std::optional<Match> CheckLatexPattern();

    /**
     * @brief Check for superscript pattern: ^digit
     */
    std::optional<Match> CheckSuperscriptPattern();

    /**
     * @brief Check for subscript pattern: _digit
     */
    std::optional<Match> CheckSubscriptPattern();

private:
    std::string m_buffer;           // Buffer of recent keystrokes
    bool m_in_superscript_mode = false;  // True when inside ^(...)
    bool m_in_subscript_mode = false;    // True when inside _(...)
    bool m_in_latex_mode = false;        // True when typing \word (to block Unikey)
    static const size_t MAX_BUFFER_SIZE = 32;  // Maximum pattern length
};

} // namespace UniLang
