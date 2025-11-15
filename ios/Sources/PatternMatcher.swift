import Foundation

/// Pattern matcher for detecting shortcuts in typed text
/// Ported from C++ pattern_matcher.cpp
public class PatternMatcher {
    private var buffer = ""
    private let triggerChar: Character = "\\"

    public init() {}

    /// Reset the pattern buffer
    public func reset() {
        buffer = ""
    }

    /// Add a character to the pattern buffer
    /// - Parameter char: The character to add
    /// - Returns: true if pattern might match, false if should reset
    public func addChar(_ char: Character) -> Bool {
        // If trigger character, start new pattern
        if char == triggerChar {
            buffer = String(char)
            return true
        }

        // If buffer is empty and not trigger char, ignore
        if buffer.isEmpty {
            return false
        }

        // Add character to buffer if it's alphanumeric
        if char.isLetter || char.isNumber || char == "_" {
            buffer.append(char)
            return true
        }

        // Non-alphanumeric character ends the pattern
        return false
    }

    /// Get current pattern in buffer
    /// - Returns: Current pattern string (e.g., "\\alpha")
    public func getCurrentPattern() -> String {
        return buffer
    }

    /// Get pattern length (for backspace calculation)
    /// - Returns: Number of characters in current pattern
    public func getPatternLength() -> Int {
        return buffer.count
    }

    /// Check if we have a potential pattern
    /// - Returns: true if buffer contains trigger char + at least one letter
    public func hasPattern() -> Bool {
        return buffer.count >= 2 && buffer.first == triggerChar
    }
}
