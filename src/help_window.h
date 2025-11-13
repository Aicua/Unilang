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
     * @brief Initialize ListView control
     */
    void InitializeListView();

    /**
     * @brief Populate ListView with all shortcuts
     */
    void PopulateListView(const std::string& filter = "");

    /**
     * @brief Handle search text change
     */
    void OnSearchTextChanged();

    /**
     * @brief Get category name for a shortcut
     */
    std::string GetCategoryForShortcut(const std::string& shortcut) const;

private:
    HWND m_hwnd = nullptr;
    HWND m_search_edit = nullptr;
    HWND m_listview = nullptr;
    HINSTANCE m_hinstance = nullptr;
    const ShortcutsDict* m_shortcuts_dict = nullptr;

    // Window dimensions
    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;
    static const int SEARCH_HEIGHT = 30;
    static const int MARGIN = 10;

    // Control IDs
    static const UINT ID_SEARCH_EDIT = 2001;
    static const UINT ID_LISTVIEW = 2002;
};

} // namespace UniLang
