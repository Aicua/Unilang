#include "pattern_matcher.h"
#include <algorithm>
#include <cctype>

namespace UniLang {

PatternMatcher::PatternMatcher() {
    m_buffer.reserve(MAX_BUFFER_SIZE);
}

std::optional<PatternMatcher::Match> PatternMatcher::AddChar(char ch) {
    // Add character to buffer
    m_buffer += ch;

    // Keep buffer size limited
    if (m_buffer.size() > MAX_BUFFER_SIZE) {
        m_buffer = m_buffer.substr(m_buffer.size() - MAX_BUFFER_SIZE);
    }

    // Enter LaTeX mode when backslash is typed
    if (ch == '\\') {
        m_in_latex_mode = true;
    }
    // Exit LaTeX mode if a non-alphabetic and non-space character is typed while in LaTeX mode
    else if (m_in_latex_mode && !std::isalpha(static_cast<unsigned char>(ch)) && ch != ' ') {
        m_in_latex_mode = false;
    }

    // Special handling for mode control
    // Check if entering superscript/subscript mode with ^( or _(
    if (m_buffer.size() >= 2) {
        std::string last_two = m_buffer.substr(m_buffer.size() - 2);
        if (last_two == "^(") {
            m_in_superscript_mode = true;
            // Return match for ^( to convert it to ⁽
            Match match;
            match.pattern = "^(";
            match.start_pos = m_buffer.size() - 2;
            match.length = 2;
            return match;
        } else if (last_two == "_(") {
            m_in_subscript_mode = true;
            // Return match for _( to convert it to ₍
            Match match;
            match.pattern = "_(";
            match.start_pos = m_buffer.size() - 2;
            match.length = 2;
            return match;
        }
    }

    // If we're in superscript/subscript mode, handle closing parenthesis
    if (ch == ')' && (m_in_superscript_mode || m_in_subscript_mode)) {
        // Create a synthetic pattern to convert ) to ⁾ or ₎
        Match match;
        match.pattern = m_in_superscript_mode ? "^)" : "_)";
        match.start_pos = m_buffer.size() - 1;
        match.length = 1;

        // Exit mode
        m_in_superscript_mode = false;
        m_in_subscript_mode = false;

        return match;
    }

    // If we're in superscript/subscript mode, auto-convert characters
    if (m_in_superscript_mode || m_in_subscript_mode) {
        // Create synthetic pattern for auto-conversion
        std::string prefix = m_in_superscript_mode ? "^" : "_";
        Match match;
        match.pattern = prefix + std::string(1, ch);
        match.start_pos = m_buffer.size() - 1;
        match.length = 1;
        return match;
    }

    // Check for pattern matches (normal mode)
    return CheckForMatch();
}

void PatternMatcher::Reset() {
    m_buffer.clear();
    m_in_superscript_mode = false;
    m_in_subscript_mode = false;
    m_in_latex_mode = false;
}

void PatternMatcher::RemoveLastChar() {
    if (!m_buffer.empty()) {
        m_buffer.pop_back();
    }
}

std::optional<PatternMatcher::Match> PatternMatcher::CheckForMatch() {
    // Try different pattern types in priority order

    // 1. LaTeX-style patterns: \word (highest priority)
    auto latex_match = CheckLatexPattern();
    if (latex_match) return latex_match;

    // 2. Superscript: ^digit
    auto super_match = CheckSuperscriptPattern();
    if (super_match) return super_match;

    // 3. Subscript: _digit
    auto sub_match = CheckSubscriptPattern();
    if (sub_match) return sub_match;

    return std::nullopt;
}

std::optional<PatternMatcher::Match> PatternMatcher::CheckLatexPattern() {
    // Look for pattern: \<alphabetic_characters> + SPACE
    // SIMPLE SPACE-TRIGGERED: Only trigger when user presses space
    // Examples: \alpha + space → α, \int + space → ∫

    if (m_buffer.empty()) {
        return std::nullopt;
    }

    char last_char = m_buffer.back();

    // ONLY trigger on SPACE
    if (last_char != ' ') {
        return std::nullopt;
    }

    // Find backslash before the space
    size_t backslash_pos = m_buffer.rfind('\\');
    if (backslash_pos == std::string::npos) {
        return std::nullopt;
    }

    // Must have at least \xx + space (minimum: "\in " = 4 chars)
    if (m_buffer.size() - backslash_pos < 4) {
        // Exit LaTeX mode - pattern too short or invalid
        m_in_latex_mode = false;
        return std::nullopt;
    }

    // Extract pattern WITHOUT the space
    std::string potential_pattern = m_buffer.substr(backslash_pos, m_buffer.size() - backslash_pos - 1);

    // Verify all chars after backslash are alphabetic
    for (size_t i = 1; i < potential_pattern.size(); ++i) {
        if (!std::isalpha(static_cast<unsigned char>(potential_pattern[i]))) {
            // Exit LaTeX mode - invalid pattern
            m_in_latex_mode = false;
            return std::nullopt; // Invalid pattern
        }
    }

    // Valid LaTeX pattern found!
    if (potential_pattern.size() >= 3) {  // At least \xx
        // Exit LaTeX mode - pattern matched
        m_in_latex_mode = false;

        Match match;
        match.pattern = potential_pattern;
        match.start_pos = backslash_pos;
        // Length includes the space trigger
        match.length = potential_pattern.size() + 1;
        return match;
    }

    // Exit LaTeX mode - pattern too short
    m_in_latex_mode = false;
    return std::nullopt;
}

std::optional<PatternMatcher::Match> PatternMatcher::CheckSuperscriptPattern() {
    // Look for pattern: ^<digit, letter, or special char>
    // Examples: ^2, ^3, ^n, ^a, ^b, ^+, ^-, ^/

    if (m_buffer.size() < 2) {
        return std::nullopt;
    }

    // Check last 2 characters
    if (m_buffer[m_buffer.size() - 2] == '^') {
        char last_char = m_buffer.back();

        // Valid superscript chars: 0-9, a-z, A-Z, +, -, =, (, ), /
        if (std::isalnum(static_cast<unsigned char>(last_char)) ||
            last_char == '+' ||
            last_char == '-' ||
            last_char == '=' ||
            last_char == '(' ||
            last_char == ')' ||
            last_char == '/') {

            Match match;
            match.pattern = m_buffer.substr(m_buffer.size() - 2);
            match.start_pos = m_buffer.size() - 2;
            match.length = 2;
            return match;
        }
    }

    return std::nullopt;
}

std::optional<PatternMatcher::Match> PatternMatcher::CheckSubscriptPattern() {
    // Look for pattern: _<digit, letter, or special char>
    // Examples: _1, _2, _a, _e, _+, _-, _/

    if (m_buffer.size() < 2) {
        return std::nullopt;
    }

    // Check last 2 characters
    if (m_buffer[m_buffer.size() - 2] == '_') {
        char last_char = m_buffer.back();

        // Valid subscript chars: 0-9, a-z, +, -, =, (, ), /
        if (std::isalnum(static_cast<unsigned char>(last_char)) ||
            last_char == '+' ||
            last_char == '-' ||
            last_char == '=' ||
            last_char == '(' ||
            last_char == ')' ||
            last_char == '/') {

            Match match;
            match.pattern = m_buffer.substr(m_buffer.size() - 2);
            match.start_pos = m_buffer.size() - 2;
            match.length = 2;
            return match;
        }
    }

    return std::nullopt;
}

} // namespace UniLang
