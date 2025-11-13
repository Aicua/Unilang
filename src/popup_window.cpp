#include "popup_window.h"
#include <iostream>

namespace UniLang {

const wchar_t* PopupWindow::WINDOW_CLASS_NAME = L"UniLangPopupWindow";

PopupWindow::PopupWindow() {
}

PopupWindow::~PopupWindow() {
    if (m_hwnd != nullptr) {
        DestroyWindow(m_hwnd);
    }
}

bool PopupWindow::Create(HINSTANCE hInstance) {
    m_hInstance = hInstance;

    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
//             std::cerr << "Failed to register popup window class. Error: " << error << std::endl;
            return false;
        }
    }

    // Create layered window
    m_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        WINDOW_CLASS_NAME,
        L"UniLang Preview",
        WS_POPUP,
        0, 0, 200, 60,
        nullptr,
        nullptr,
        hInstance,
        this  // Pass this pointer to WM_CREATE
    );

    if (m_hwnd == nullptr) {
        DWORD error = GetLastError();
//         std::cerr << "Failed to create popup window. Error: " << error << std::endl;
        return false;
    }

    // Set window opacity
    SetLayeredWindowAttributes(m_hwnd, 0, 230, LWA_ALPHA);

    return true;
}

void PopupWindow::Show(const std::string& shortcut, const std::string& replacement, int duration_ms) {
    if (m_hwnd == nullptr) {
        return;
    }

    m_shortcut = shortcut;
    m_replacement = replacement;

    // Update position near cursor
    UpdatePosition();

    // Show window
    ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
    m_visible = true;

    // Force repaint
    InvalidateRect(m_hwnd, nullptr, TRUE);
    UpdateWindow(m_hwnd);

    // Set timer to auto-hide
    SetTimer(m_hwnd, TIMER_ID, duration_ms, nullptr);
}

void PopupWindow::Hide() {
    if (m_hwnd != nullptr && m_visible) {
        ShowWindow(m_hwnd, SW_HIDE);
        m_visible = false;
        KillTimer(m_hwnd, TIMER_ID);
    }
}

void PopupWindow::UpdatePosition() {
    POINT cursor_pos;
    GetCursorPos(&cursor_pos);

    // Position popup slightly below and to the right of cursor
    int x = cursor_pos.x + 20;
    int y = cursor_pos.y + 20;

    SetWindowPos(m_hwnd, HWND_TOPMOST, x, y, 200, 60,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void PopupWindow::Render(HDC hdc) {
    // Get window rect
    RECT rect;
    GetClientRect(m_hwnd, &rect);

    // Fill background with light yellow
    HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 220));
    FillRect(hdc, &rect, bgBrush);
    DeleteObject(bgBrush);

    // Draw border
    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);

    // Set text properties
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));

    // Create font
    HFONT font = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                            L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);

    // Draw shortcut text
    std::wstring shortcut_w(m_shortcut.begin(), m_shortcut.end());
    TextOutW(hdc, 10, 10, shortcut_w.c_str(), static_cast<int>(shortcut_w.length()));

    // Draw arrow
    TextOutW(hdc, 10, 30, L"→", 1);

    // Draw replacement (larger font)
    HFONT largeFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                 L"Segoe UI");
    SelectObject(hdc, largeFont);

    // Convert UTF-8 to UTF-16 for display
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, m_replacement.c_str(),
                                         static_cast<int>(m_replacement.size()),
                                         nullptr, 0);
    std::wstring replacement_w(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, m_replacement.c_str(),
                       static_cast<int>(m_replacement.size()),
                       &replacement_w[0], size_needed);

    SetTextColor(hdc, RGB(0, 100, 200));
    TextOutW(hdc, 40, 25, replacement_w.c_str(), static_cast<int>(replacement_w.length()));

    // Cleanup
    SelectObject(hdc, oldFont);
    DeleteObject(font);
    DeleteObject(largeFont);
}

void PopupWindow::OnTimer() {
    Hide();
}

LRESULT CALLBACK PopupWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PopupWindow* pThis = nullptr;

    if (msg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<PopupWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    } else {
        pThis = reinterpret_cast<PopupWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (pThis) {
                pThis->Render(hdc);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_TIMER: {
            if (pThis && wParam == TIMER_ID) {
                pThis->OnTimer();
            }
            return 0;
        }

        case WM_CLOSE:
            if (pThis) {
                pThis->Hide();
            }
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

} // namespace UniLang
