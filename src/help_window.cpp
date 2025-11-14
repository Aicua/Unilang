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

    // Create window as compact popup
    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"UniLangHelpWindow",
        L"Search Shortcuts",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
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
        WINDOW_WIDTH - 2 * MARGIN, SEARCH_HEIGHT,
        m_hwnd,
        (HMENU)ID_SEARCH_EDIT,
        hInstance,
        nullptr
    );

    // Set placeholder text
    SendMessageW(m_search_edit, EM_SETCUEBANNER, TRUE, (LPARAM)L"Search...");

    // Create ListBox for results
    m_listbox = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"LISTBOX",
        L"",
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_HASSTRINGS | WS_VSCROLL,
        MARGIN, SEARCH_HEIGHT + 2 * MARGIN,
        WINDOW_WIDTH - 2 * MARGIN, WINDOW_HEIGHT - SEARCH_HEIGHT - 3 * MARGIN,
        m_hwnd,
        (HMENU)ID_LISTBOX,
        hInstance,
        nullptr
    );

    // Set font for better Unicode display
    HFONT hFont = CreateFontW(
        18,                        // Height
        0,                         // Width
        0,                         // Escapement
        0,                         // Orientation
        FW_NORMAL,                 // Weight
        FALSE,                     // Italic
        FALSE,                     // Underline
        FALSE,                     // StrikeOut
        DEFAULT_CHARSET,           // CharSet
        OUT_DEFAULT_PRECIS,        // OutPrecision
        CLIP_DEFAULT_PRECIS,       // ClipPrecision
        CLEARTYPE_QUALITY,         // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        L"Segoe UI"                // Font name
    );
    SendMessageW(m_listbox, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Populate with all shortcuts initially
    PopulateListBox();

    return true;
}

std::wstring HelpWindow::Utf8ToWide(const std::string& utf8) const {
    if (utf8.empty()) return L"";

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), nullptr, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), &result[0], size_needed);
    return result;
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

void HelpWindow::PopulateListBox(const std::string& filter) {
    // Clear existing items
    SendMessageW(m_listbox, LB_RESETCONTENT, 0, 0);

    if (!m_shortcuts_dict) return;

    // Get all shortcuts
    const auto& shortcuts = m_shortcuts_dict->GetAllShortcuts();

    // Convert filter to lowercase for case-insensitive search
    std::string filter_lower = filter;
    std::transform(filter_lower.begin(), filter_lower.end(), filter_lower.begin(), ::tolower);

    int item_count = 0;
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

        // Format: "symbol  -  shortcut  (description)"
        // Example: "α  -  \al  (alpha)"
        std::string display_text = replacement + "  -  " + shortcut;
        if (!description.empty()) {
            display_text += "  (" + description + ")";
        }

        // Convert to wide string properly using UTF-8 conversion
        std::wstring display_text_w = Utf8ToWide(display_text);

        // Add to listbox
        SendMessageW(m_listbox, LB_ADDSTRING, 0, (LPARAM)display_text_w.c_str());

        item_count++;
    }

    // Update window title with count
    std::wstring title = L"Search Shortcuts";
    if (item_count > 0) {
        title += L" (" + std::to_wstring(item_count) + L")";
    }
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
    PopulateListBox(search_text);
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

            SetWindowPos(pThis->m_listbox, nullptr,
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
