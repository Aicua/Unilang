#pragma once

#include <string>
#include <unordered_map>
#include <optional>

namespace UniLang {

/**
 * @brief Manages the shortcuts dictionary for text replacement
 *
 * Loads shortcuts from JSON config file and provides lookup functionality.
 * Example: "\alpha" -> "α"
 */
class ShortcutsDict {
public:
    ShortcutsDict();
    ~ShortcutsDict() = default;

    /**
     * @brief Load shortcuts from embedded Windows resource
     * @return true if loaded successfully
     */
    bool LoadFromResource();

    /**
     * @brief Load shortcuts from JSON file
     * @param filepath Path to shortcuts.json
     * @return true if loaded successfully
     */
    bool LoadFromFile(const std::string& filepath);

    /**
     * @brief Find replacement for a shortcut
     * @param shortcut The shortcut to look up (e.g., "\\alpha")
     * @return The Unicode replacement if found, nullopt otherwise
     */
    std::optional<std::string> FindReplacement(const std::string& shortcut) const;

    /**
     * @brief Get all shortcuts (for UI display)
     * @return Map of shortcuts to replacements
     */
    const std::unordered_map<std::string, std::string>& GetAllShortcuts() const;

    /**
     * @brief Check if shortcuts dictionary is loaded
     */
    bool IsLoaded() const { return m_loaded; }

    /**
     * @brief Get total number of shortcuts
     */
    size_t GetShortcutCount() const { return m_shortcuts.size(); }

private:
    std::unordered_map<std::string, std::string> m_shortcuts;
    bool m_loaded = false;
};

} // namespace UniLang
