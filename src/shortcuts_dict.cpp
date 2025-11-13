#include "shortcuts_dict.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include "resource.h"

using json = nlohmann::json;

namespace UniLang {

ShortcutsDict::ShortcutsDict() {
}

bool ShortcutsDict::LoadFromResource() {
    try {
        // Find the JSON resource
        HRSRC hResource = FindResourceW(nullptr, MAKEINTRESOURCEW(IDR_SHORTCUTS_JSON), L"JSON");
        if (!hResource) {
            return false;
        }

        // Load the resource
        HGLOBAL hLoadedResource = LoadResource(nullptr, hResource);
        if (!hLoadedResource) {
            return false;
        }

        // Lock the resource to get a pointer to the data
        LPVOID pResourceData = LockResource(hLoadedResource);
        if (!pResourceData) {
            return false;
        }

        // Get resource size
        DWORD dwResourceSize = SizeofResource(nullptr, hResource);
        if (dwResourceSize == 0) {
            return false;
        }

        // Parse JSON from memory
        std::string jsonData(static_cast<const char*>(pResourceData), dwResourceSize);
        json j = json::parse(jsonData);

        // Clear existing shortcuts
        m_shortcuts.clear();

        // Load all shortcuts from nested structure
        if (j.contains("shortcuts")) {
            auto shortcuts_obj = j["shortcuts"];

            // Iterate through all categories
            for (auto& category : shortcuts_obj.items()) {
                if (category.key() == "_comment") continue;

                if (category.value().is_object()) {
                    // This is a category like "greek_lowercase", "math_operators", etc.
                    for (auto& item : category.value().items()) {
                        std::string shortcut = item.key();
                        std::string replacement = item.value();
                        m_shortcuts[shortcut] = replacement;
                    }
                }
            }
        }

        m_loaded = true;
        // std::cout << "Loaded " << m_shortcuts.size() << " shortcuts from resource" << std::endl;
        return true;

    } catch (const std::exception& e) {
        // std::cerr << "Error loading shortcuts from resource: " << e.what() << std::endl;
        return false;
    }
}

bool ShortcutsDict::LoadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            // std::cerr << "Failed to open shortcuts file: " << filepath << std::endl;
            return false;
        }

        json j;
        file >> j;

        // Clear existing shortcuts
        m_shortcuts.clear();

        // Load all shortcuts from nested structure
        if (j.contains("shortcuts")) {
            auto shortcuts_obj = j["shortcuts"];

            // Iterate through all categories
            for (auto& category : shortcuts_obj.items()) {
                if (category.key() == "_comment") continue;

                if (category.value().is_object()) {
                    // This is a category like "greek_lowercase", "math_operators", etc.
                    for (auto& item : category.value().items()) {
                        std::string shortcut = item.key();
                        std::string replacement = item.value();
                        m_shortcuts[shortcut] = replacement;
                    }
                }
            }
        }

        m_loaded = true;
        // std::cout << "Loaded " << m_shortcuts.size() << " shortcuts from " << filepath << std::endl;
        return true;

    } catch (const std::exception& e) {
        // std::cerr << "Error loading shortcuts: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::string> ShortcutsDict::FindReplacement(const std::string& shortcut) const {
    auto it = m_shortcuts.find(shortcut);
    if (it != m_shortcuts.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::unordered_map<std::string, std::string>& ShortcutsDict::GetAllShortcuts() const {
    return m_shortcuts;
}

} // namespace UniLang
