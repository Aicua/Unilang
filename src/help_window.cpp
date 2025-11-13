#include "help_window.h"
#include "shortcuts_dict.h"
#include <algorithm>
#include <sstream>

namespace UniLang {

HelpWindow::HelpWindow() {
}

HelpWindow::~HelpWindow() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

bool HelpWindow::Create(HINSTANCE hInstance, const ShortcutsDict* shortcuts_dict) {
    m_hinstance = hInstance;
    m_shortcuts_dict = shortcuts_dict;

    if (!m_shortcuts_dict || !m_shortcuts_dict->IsLoaded()) {
        return false;
    }

    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"UniLangHelpWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }

    // Create window
    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST,
        L"UniLangHelpWindow",
        L"UniLang - Shortcuts Reference",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, this
    );

    if (!m_hwnd) {
        return false;
    }

    // Create search edit box
    m_search_edit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
        MARGIN, MARGIN,
        WINDOW_WIDTH - 2 * MARGIN - 20, SEARCH_HEIGHT,
        m_hwnd,
        (HMENU)ID_SEARCH_EDIT,
        hInstance,
        nullptr
    );

    // Set placeholder text
    SendMessageW(m_search_edit, EM_SETCUEBANNER, TRUE, (LPARAM)L"Search shortcuts... (e.g., alpha, sum, arrow)");

    // Create ListView
    m_listview = CreateWindowExW(
        0,
        WC_LISTVIEWW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
        MARGIN, SEARCH_HEIGHT + 2 * MARGIN,
        WINDOW_WIDTH - 2 * MARGIN - 20, WINDOW_HEIGHT - SEARCH_HEIGHT - 3 * MARGIN - 40,
        m_hwnd,
        (HMENU)ID_LISTVIEW,
        hInstance,
        nullptr
    );

    // Initialize ListView
    InitializeListView();
    PopulateListView();

    return true;
}

void HelpWindow::InitializeListView() {
    // Set extended styles
    ListView_SetExtendedListViewStyle(m_listview,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    // Add columns
    LVCOLUMNW col = {};
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    col.fmt = LVCFMT_LEFT;

    // Column 1: Category
    col.cx = 180;
    col.pszText = (LPWSTR)L"Category";
    ListView_InsertColumn(m_listview, 0, &col);

    // Column 2: Shortcut
    col.cx = 200;
    col.pszText = (LPWSTR)L"Shortcut";
    ListView_InsertColumn(m_listview, 1, &col);

    // Column 3: Symbol
    col.cx = 100;
    col.pszText = (LPWSTR)L"Symbol";
    ListView_InsertColumn(m_listview, 2, &col);

    // Column 4: Description
    col.cx = 250;
    col.pszText = (LPWSTR)L"Description";
    ListView_InsertColumn(m_listview, 3, &col);
}

std::string HelpWindow::GetCategoryForShortcut(const std::string& shortcut) const {
    // Determine category based on shortcut pattern
    if (shortcut.find("\\") == 0) {
        std::string key = shortcut.substr(1);

        // Greek letters (lowercase)
        if (key == "al" || key == "be" || key == "ga" || key == "de" || key == "ep" ||
            key == "ze" || key == "eta" || key == "the" || key == "io" || key == "ka" ||
            key == "la" || key == "mu" || key == "nu" || key == "xi" || key == "pi" ||
            key == "rho" || key == "si" || key == "tau" || key == "up" || key == "phi" ||
            key == "chi" || key == "psi" || key == "om") {
            return "Greek (Lowercase)";
        }

        // Greek letters (uppercase)
        if (key == "Ga" || key == "De" || key == "The" || key == "La" || key == "Xi" ||
            key == "Pi" || key == "Si" || key == "Phi" || key == "Psi" || key == "Om") {
            return "Greek (Uppercase)";
        }

        // Math operators
        if (key == "par" || key == "nab" || key == "int" || key == "iint" || key == "iiint" ||
            key == "oint" || key == "sum" || key == "prod" || key == "sqr" || key == "inf" ||
            key == "pm" || key == "mp" || key == "tim" || key == "div" || key == "cdot" || key == "bul") {
            return "Math Operators";
        }

        // Comparison
        if (key == "app" || key == "neq" || key == "leq" || key == "geq" || key == "ll" ||
            key == "gg" || key == "equi" || key == "sim" || key == "prop") {
            return "Comparison";
        }

        // Arrows
        if (key.find("arrow") != std::string::npos) {
            return "Arrows";
        }

        // Set theory & logic
        if (key == "in" || key == "notin" || key == "subset" || key == "supset" ||
            key == "subseteq" || key == "supseteq" || key == "cup" || key == "cap" ||
            key == "emptyset" || key == "forall" || key == "exists" || key == "neg" ||
            key == "land" || key == "lor") {
            return "Sets & Logic";
        }

        // Fractions
        if (key == "half" || key == "third" || key == "quarter" || key == "twothirds" || key == "threequarters") {
            return "Fractions";
        }

        // Misc
        if (key == "deg" || key == "do" || key == "micro" || key == "angstrom" ||
            key == "ohm" || key == "euro" || key == "pound" || key == "yen") {
            return "Miscellaneous";
        }
    }

    // Superscripts
    if (shortcut.find("^") == 0) {
        return "Superscripts";
    }

    // Subscripts
    if (shortcut.find("_") == 0) {
        return "Subscripts";
    }

    return "Other";
}

std::string HelpWindow::GetDescriptionForShortcut(const std::string& shortcut) const {
    // Map shortcuts to their full names for better searchability
    if (shortcut.find("\\") == 0) {
        std::string key = shortcut.substr(1);

        // Greek letters (lowercase) - full names
        if (key == "al") return "alpha";
        if (key == "be") return "beta";
        if (key == "ga") return "gamma";
        if (key == "de") return "delta";
        if (key == "ep") return "epsilon";
        if (key == "ze") return "zeta";
        if (key == "eta") return "eta";
        if (key == "the") return "theta";
        if (key == "io") return "iota";
        if (key == "ka") return "kappa";
        if (key == "la") return "lambda";
        if (key == "mu") return "mu";
        if (key == "nu") return "nu";
        if (key == "xi") return "xi";
        if (key == "pi") return "pi";
        if (key == "rho") return "rho";
        if (key == "si") return "sigma";
        if (key == "tau") return "tau";
        if (key == "up") return "upsilon";
        if (key == "phi") return "phi";
        if (key == "chi") return "chi";
        if (key == "psi") return "psi";
        if (key == "om") return "omega";

        // Greek letters (uppercase)
        if (key == "Ga") return "Gamma";
        if (key == "De") return "Delta";
        if (key == "The") return "Theta";
        if (key == "La") return "Lambda";
        if (key == "Xi") return "Xi";
        if (key == "Pi") return "Pi";
        if (key == "Si") return "Sigma";
        if (key == "Phi") return "Phi";
        if (key == "Psi") return "Psi";
        if (key == "Om") return "Omega";

        // Math operators
        if (key == "par") return "partial derivative";
        if (key == "nab") return "nabla / del";
        if (key == "int") return "integral";
        if (key == "iint") return "double integral";
        if (key == "iiint") return "triple integral";
        if (key == "oint") return "contour integral";
        if (key == "sum") return "summation";
        if (key == "prod") return "product";
        if (key == "sqr") return "square root";
        if (key == "inf") return "infinity";
        if (key == "pm") return "plus-minus";
        if (key == "mp") return "minus-plus";
        if (key == "tim") return "times / multiply";
        if (key == "div") return "divide";
        if (key == "cdot") return "center dot";
        if (key == "bul") return "bullet";

        // Comparison
        if (key == "app") return "approximately equal";
        if (key == "neq") return "not equal";
        if (key == "leq") return "less than or equal";
        if (key == "geq") return "greater than or equal";
        if (key == "ll") return "much less than";
        if (key == "gg") return "much greater than";
        if (key == "equi") return "equivalent";
        if (key == "sim") return "similar";
        if (key == "prop") return "proportional";

        // Arrows
        if (key == "rarrow") return "right arrow";
        if (key == "larrow") return "left arrow";
        if (key == "uarrow") return "up arrow";
        if (key == "darrow") return "down arrow";
        if (key == "lrarrow") return "left-right arrow";
        if (key == "Rarrow") return "right double arrow / implies";
        if (key == "Larrow") return "left double arrow";
        if (key == "Lrarrow") return "left-right double arrow / iff";

        // Sets & Logic
        if (key == "in") return "element of / in";
        if (key == "notin") return "not element of";
        if (key == "subset") return "subset";
        if (key == "supset") return "superset";
        if (key == "subseteq") return "subset or equal";
        if (key == "supseteq") return "superset or equal";
        if (key == "cup") return "union";
        if (key == "cap") return "intersection";
        if (key == "emptyset") return "empty set";
        if (key == "forall") return "for all";
        if (key == "exists") return "exists / there exists";
        if (key == "neg") return "not / negation";
        if (key == "land") return "and / logical and";
        if (key == "lor") return "or / logical or";

        // Fractions
        if (key == "half") return "one half";
        if (key == "third") return "one third";
        if (key == "quarter") return "one quarter";
        if (key == "twothirds") return "two thirds";
        if (key == "threequarters") return "three quarters";

        // Misc
        if (key == "deg" || key == "do") return "degree";
        if (key == "micro") return "micro";
        if (key == "angstrom") return "angstrom";
        if (key == "ohm") return "ohm";
        if (key == "euro") return "euro";
        if (key == "pound") return "pound";
        if (key == "yen") return "yen";
    }

    return "";
}

void HelpWindow::PopulateListView(const std::string& filter) {
    // Clear existing items
    ListView_DeleteAllItems(m_listview);

    if (!m_shortcuts_dict) return;

    // Get all shortcuts
    const auto& shortcuts = m_shortcuts_dict->GetAllShortcuts();

    // Convert filter to lowercase for case-insensitive search
    std::string filter_lower = filter;
    std::transform(filter_lower.begin(), filter_lower.end(), filter_lower.begin(), ::tolower);

    int item_index = 0;
    for (const auto& [shortcut, replacement] : shortcuts) {
        // Get description
        std::string description = GetDescriptionForShortcut(shortcut);

        // Filter logic
        if (!filter_lower.empty()) {
            std::string shortcut_lower = shortcut;
            std::string replacement_lower = replacement;
            std::string description_lower = description;
            std::transform(shortcut_lower.begin(), shortcut_lower.end(), shortcut_lower.begin(), ::tolower);
            std::transform(replacement_lower.begin(), replacement_lower.end(), replacement_lower.begin(), ::tolower);
            std::transform(description_lower.begin(), description_lower.end(), description_lower.begin(), ::tolower);

            // Check if filter matches shortcut, replacement, or description
            if (shortcut_lower.find(filter_lower) == std::string::npos &&
                replacement_lower.find(filter_lower) == std::string::npos &&
                description_lower.find(filter_lower) == std::string::npos) {
                continue;
            }
        }

        // Get category
        std::string category = GetCategoryForShortcut(shortcut);

        // Convert to wide strings
        std::wstring category_w(category.begin(), category.end());
        std::wstring shortcut_w(shortcut.begin(), shortcut.end());
        std::wstring replacement_w(replacement.begin(), replacement.end());
        std::wstring description_w(description.begin(), description.end());

        // Insert item
        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = item_index;

        // Category
        item.iSubItem = 0;
        item.pszText = (LPWSTR)category_w.c_str();
        ListView_InsertItem(m_listview, &item);

        // Shortcut
        item.iSubItem = 1;
        item.pszText = (LPWSTR)shortcut_w.c_str();
        ListView_SetItem(m_listview, &item);

        // Symbol
        item.iSubItem = 2;
        item.pszText = (LPWSTR)replacement_w.c_str();
        ListView_SetItem(m_listview, &item);

        // Description
        item.iSubItem = 3;
        item.pszText = (LPWSTR)description_w.c_str();
        ListView_SetItem(m_listview, &item);

        item_index++;
    }

    // Update count in window title
    std::wstring title = L"UniLang - Shortcuts Reference (" +
                         std::to_wstring(item_index) + L" shortcuts";
    if (!filter.empty()) {
        title += L" - filtered";
    }
    title += L")";
    SetWindowTextW(m_hwnd, title.c_str());
}

void HelpWindow::OnSearchTextChanged() {
    // Get search text
    wchar_t buffer[256] = {};
    GetWindowTextW(m_search_edit, buffer, 256);

    // Convert to narrow string
    std::wstring ws(buffer);
    std::string search_text(ws.begin(), ws.end());

    // Repopulate with filter
    PopulateListView(search_text);
}

void HelpWindow::Show() {
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_SHOW);
        SetForegroundWindow(m_hwnd);
        SetFocus(m_search_edit);  // Focus on search box
    }
}

void HelpWindow::Hide() {
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_HIDE);
    }
}

void HelpWindow::Toggle() {
    if (IsVisible()) {
        Hide();
    } else {
        Show();
    }
}

bool HelpWindow::IsVisible() const {
    return m_hwnd && IsWindowVisible(m_hwnd);
}

LRESULT CALLBACK HelpWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HelpWindow* pThis = nullptr;

    if (msg == WM_CREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (HelpWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (HelpWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_SEARCH_EDIT && HIWORD(wParam) == EN_CHANGE) {
                pThis->OnSearchTextChanged();
            }
            break;

        case WM_CLOSE:
            pThis->Hide();
            return 0;

        case WM_SIZE: {
            // Resize controls when window is resized
            RECT rc;
            GetClientRect(hwnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;

            SetWindowPos(pThis->m_search_edit, nullptr,
                MARGIN, MARGIN,
                width - 2 * MARGIN, SEARCH_HEIGHT,
                SWP_NOZORDER);

            SetWindowPos(pThis->m_listview, nullptr,
                MARGIN, SEARCH_HEIGHT + 2 * MARGIN,
                width - 2 * MARGIN,
                height - SEARCH_HEIGHT - 3 * MARGIN,
                SWP_NOZORDER);
            break;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                pThis->Hide();
                return 0;
            }
            break;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

} // namespace UniLang
