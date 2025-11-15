package com.aicua.unilang

/**
 * Pattern matcher for detecting shortcuts in typed text
 * Ported from C++ pattern_matcher.cpp
 */
class PatternMatcher {
    private val buffer = StringBuilder()
    private var triggerChar = '\\'

    /**
     * Reset the pattern buffer
     */
    fun reset() {
        buffer.clear()
    }

    /**
     * Add a character to the pattern buffer
     * @param char The character to add
     * @return true if pattern might match, false if should reset
     */
    fun addChar(char: Char): Boolean {
        // If trigger character, start new pattern
        if (char == triggerChar) {
            buffer.clear()
            buffer.append(char)
            return true
        }

        // If buffer is empty and not trigger char, ignore
        if (buffer.isEmpty()) {
            return false
        }

        // Add character to buffer if it's alphanumeric
        if (char.isLetterOrDigit() || char == '_') {
            buffer.append(char)
            return true
        }

        // Non-alphanumeric character ends the pattern
        return false
    }

    /**
     * Get current pattern in buffer
     * @return Current pattern string (e.g., "\\alpha")
     */
    fun getCurrentPattern(): String {
        return buffer.toString()
    }

    /**
     * Get pattern length (for backspace calculation)
     * @return Number of characters in current pattern
     */
    fun getPatternLength(): Int {
        return buffer.length
    }

    /**
     * Check if we have a potential pattern
     * @return true if buffer contains trigger char + at least one letter
     */
    fun hasPattern(): Boolean {
        return buffer.length >= 2 && buffer[0] == triggerChar
    }
}
