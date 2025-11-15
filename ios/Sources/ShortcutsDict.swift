import Foundation

/// Manages the shortcuts dictionary for text replacement
/// Ported from C++ shortcuts_dict.cpp
///
/// Example: "\\alpha" -> "α"
public class ShortcutsDict {
    private var shortcuts: [String: String] = [:]
    private var loaded = false

    public init() {}

    /// Load shortcuts from JSON file
    /// - Parameter filepath: Path to shortcuts.json
    /// - Returns: true if loaded successfully
    public func loadFromFile(filepath: String) -> Bool {
        guard let data = try? Data(contentsOf: URL(fileURLWithPath: filepath)) else {
            return false
        }

        return loadFromData(data)
    }

    /// Load shortcuts from JSON data
    /// - Parameter data: JSON data
    /// - Returns: true if loaded successfully
    public func loadFromData(_ data: Data) -> Bool {
        do {
            guard let json = try JSONSerialization.jsonObject(with: data) as? [String: Any],
                  let shortcutsObj = json["shortcuts"] as? [String: Any] else {
                return false
            }

            shortcuts.removeAll()

            // Iterate through all categories
            for (categoryKey, categoryValue) in shortcutsObj {
                if categoryKey == "_comment" { continue }

                if let categoryDict = categoryValue as? [String: String] {
                    // Load shortcuts from this category
                    for (shortcut, replacement) in categoryDict {
                        shortcuts[shortcut] = replacement
                    }
                }
            }

            loaded = true
            return true

        } catch {
            print("Error loading shortcuts: \(error)")
            return false
        }
    }

    /// Find replacement for a shortcut
    /// - Parameter shortcut: The shortcut to look up (e.g., "\\alpha")
    /// - Returns: The Unicode replacement if found, nil otherwise
    public func findReplacement(_ shortcut: String) -> String? {
        return shortcuts[shortcut]
    }

    /// Get all shortcuts (for UI display)
    /// - Returns: Dictionary of shortcuts to replacements
    public func getAllShortcuts() -> [String: String] {
        return shortcuts
    }

    /// Check if shortcuts dictionary is loaded
    public func isLoaded() -> Bool {
        return loaded
    }

    /// Get total number of shortcuts
    public func getShortcutCount() -> Int {
        return shortcuts.count
    }
}
