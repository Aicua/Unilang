#pragma once

/**
 * @file platform_types.h
 * @brief Cross-platform type definitions and macros
 */

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
    #define PLATFORM_LINUX 0
    #define PLATFORM_MACOS 0
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_WINDOWS 0
    #define PLATFORM_LINUX 0
    #define PLATFORM_MACOS 1
#elif defined(__linux__)
    #define PLATFORM_WINDOWS 0
    #define PLATFORM_LINUX 1
    #define PLATFORM_MACOS 0
#else
    #error "Unsupported platform"
#endif

// Platform-specific includes
#if PLATFORM_WINDOWS
    #include <Windows.h>
    using PlatformWindow = HWND;
    using PlatformInstance = HINSTANCE;
#elif PLATFORM_LINUX
    #include <cstdint>
    using PlatformWindow = void*;  // Will be xcb_window_t or similar
    using PlatformInstance = void*;
#elif PLATFORM_MACOS
    #include <cstdint>
    using PlatformWindow = void*;  // Will be NSWindow* or similar
    using PlatformInstance = void*;
#endif

namespace UniLang {
namespace Platform {

/**
 * @brief Virtual key codes (cross-platform)
 */
enum class VirtualKey : uint32_t {
    Unknown = 0,
    Backspace = 8,
    Tab = 9,
    Enter = 13,
    Shift = 16,
    Control = 17,
    Alt = 18,
    Escape = 27,
    Space = 32,
    Left = 37,
    Up = 38,
    Right = 39,
    Down = 40,
    Delete = 46,

    // Letters A-Z (65-90)
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Numbers 0-9 (48-57)
    Num0 = 48, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,
};

} // namespace Platform
} // namespace UniLang
