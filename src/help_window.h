#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>

namespace UniLang {

class ShortcutsDict;

/**
 * @brief Help window showing searchable shortcuts list
 *
 * Features:
 * - Search box for filtering shortcuts
 * - List view with Category, Shortcut, and Symbol columns
 * - Shows all 200+ shortcuts in organized categories
 */
class HelpWindow {
public:
    HelpWindow();
    ~HelpWindow();

    /**
     * @brief Create the help window
     * @param hInstance Application instance
     * @param shortcuts_dict Reference to shortcuts dictionary
     * @return true if successful
     */
    bool Create(HINSTANCE hInstance, const ShortcutsDict* shortcuts_dict);

    /**
     * @brief Show the help window
     */
    void Show();

    /**
     * @brief Hide the help window
     */
    void Hide();

    /**
     * @brief Toggle visibility
     */
    void Toggle();

    /**
     * @brief Check if window is visible
     */
    bool IsVisible() const;

    /**
     * @brief Get window handle
     */
    HWND GetHandle() const { return m_hwnd; }

private:
    /**
     * @brief Window procedure
     */
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Populate ListBox with shortcuts (filtered or all)
     */
    void PopulateListBox(const std::string& filter = "");

    /**
     * @brief Handle search text change
     */
    void OnSearchTextChanged();

    /**
     * @brief Get category name for a shortcut
     */
    std::string GetCategoryForShortcut(const std::string& shortcut) const;

    /**
     * @brief Get description/full name for a shortcut (e.g., "\al" → "alpha")
     */
    std::string GetDescriptionForShortcut(const std::string& shortcut) const;

    /**
     * @brief Convert UTF-8 string to wide string
     */
    std::wstring Utf8ToWide(const std::string& utf8) const;

    /**
     * @brief Check if a replacement string is a URL
     */
    bool IsURL(const std::string& text) const;

    /**
     * @brief Handle listbox item click
     */
    void OnListBoxClick();

private:
    HWND m_hwnd = nullptr;
    HWND m_search_edit = nullptr;
    HWND m_listbox = nullptr;
    HINSTANCE m_hinstance = nullptr;
    const ShortcutsDict* m_shortcuts_dict = nullptr;

    // Map listbox index to URL (for clickable URLs)
    std::vector<std::string> m_item_urls;

    // Window dimensions - compact popup style
    static const int WINDOW_WIDTH = 400;
    static const int WINDOW_HEIGHT = 300;  // Increased for more results
    static const int SEARCH_HEIGHT = 30;
    static const int MARGIN = 8;
    static const int MAX_RESULTS = 10;

    // Control IDs
    static const UINT ID_SEARCH_EDIT = 2001;
    static const UINT ID_LISTBOX = 2002;
};

} // namespace UniLang
