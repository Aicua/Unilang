# UniLang Android - Keyboard IME

LaTeX-style Math & Symbol Shortcuts keyboard for Android.

## Features

- ✅ Custom keyboard (Input Method Editor)
- ✅ LaTeX-style shortcuts (`\alpha` → `α`, `\sum` → `∑`)
- ✅ 200+ mathematical symbols and Greek letters
- ✅ Works in any Android app
- ✅ Ported from desktop C++ codebase

## Requirements

- Android 7.0 (API 24) or higher
- Android Studio (for development)

## Building

### Using Android Studio

1. Open `android/` folder in Android Studio
2. Wait for Gradle sync to complete
3. Build → Build APK
4. APK will be in `app/build/outputs/apk/debug/`

### Using Command Line

```bash
cd android
./gradlew assembleDebug
```

APK location: `app/build/outputs/apk/debug/app-debug.apk`

## Installation

### From APK

```bash
adb install app/build/outputs/apk/debug/app-debug.apk
```

### Enable Keyboard

1. Go to **Settings** → **System** → **Languages & Input**
2. Tap **Virtual keyboard** → **Manage keyboards**
3. Enable **UniLang Keyboard**
4. In any text field, long-press the spacebar to switch keyboards
5. Select **UniLang**

## Usage

Once enabled, use UniLang keyboard in any app:

1. Switch to UniLang keyboard
2. Type shortcuts: `\alpha`, `\sum`, `\pi`, etc.
3. Shortcuts are automatically replaced with symbols

### Example Shortcuts

- `\alpha` → α
- `\beta` → β
- `\gamma` → γ
- `\sum` → ∑
- `\int` → ∫
- `\pi` → π
- `\infty` → ∞

Full list: See `app/src/main/assets/shortcuts.json`

## Architecture

```
UniLangIME (InputMethodService)
├── ShortcutsDict.kt          # Manages shortcuts (ported from C++)
├── PatternMatcher.kt         # Detects patterns (ported from C++)
└── MainActivity.kt           # Settings UI
```

### Key Components

**ShortcutsDict.kt**
- Loads shortcuts from JSON
- Provides lookup functionality
- Ported from `src/shortcuts_dict.cpp`

**PatternMatcher.kt**
- Detects shortcut patterns as user types
- Buffer management
- Ported from `src/pattern_matcher.cpp`

**UniLangIME.kt**
- Main keyboard service
- Handles key events
- Performs text replacement

## Comparison with Desktop

| Feature | Desktop (C++) | Android (Kotlin) |
|---------|--------------|------------------|
| **Method** | Background app + hooks | Custom keyboard IME |
| **Pattern matching** | ✅ Shared logic | ✅ Ported |
| **Shortcuts dict** | ✅ Shared JSON | ✅ Same file |
| **System-wide** | ✅ All apps | ✅ All apps (when keyboard active) |

## Development

### Project Structure

```
android/
├── app/
│   ├── src/main/
│   │   ├── java/com/aicua/unilang/
│   │   │   ├── UniLangIME.kt
│   │   │   ├── ShortcutsDict.kt
│   │   │   ├── PatternMatcher.kt
│   │   │   └── MainActivity.kt
│   │   ├── res/
│   │   │   ├── layout/
│   │   │   ├── xml/
│   │   │   └── values/
│   │   └── assets/
│   │       └── shortcuts.json
│   └── build.gradle
├── build.gradle
└── settings.gradle
```

### Adding New Shortcuts

Edit `app/src/main/assets/shortcuts.json`:

```json
{
  "shortcuts": {
    "custom": {
      "\\myshortcut": "→"
    }
  }
}
```

## Testing

1. Install APK on device/emulator
2. Enable UniLang keyboard
3. Open any text editor app
4. Switch to UniLang keyboard
5. Type `\alpha` and verify it replaces with `α`

## Publishing

To publish to Google Play Store:

1. Create signed release APK
2. Update version in `app/build.gradle`
3. Submit to Play Console
4. Wait for review

## License

MIT License - See LICENSE file for details
