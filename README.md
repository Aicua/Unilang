# UniLang - Universal Language & Text Replacement Tool

## Overview
UniLang is a powerful Windows application that provides LaTeX-style shortcuts for typing mathematical symbols, Greek letters, and special characters. Type `\alpha` and get `α`, type `\sum` and get `∑` - it's that simple!

## Features
- ⌨️ **LaTeX-style Shortcuts**: Type `\pi` → `π`, `\sum` → `∑`, `\alpha` → `α`
- 🔄 **Real-time Text Replacement**: Automatic text replacement as you type
- 🪟 **System-wide Integration**: Works in any Windows application
- 💬 **Smart Popup Suggestions**: Visual feedback for available shortcuts
- ⚙️ **Customizable**: Add your own shortcuts via JSON configuration
- 🎯 **Pattern Matching**: Intelligent detection and replacement

## Requirements
- **OS**: Windows 10 or later
- **Build Tools**: Visual Studio 2019/2022 or CMake + MinGW
- **CMake**: Version 3.16 or higher

## Building from Source

### Using CMake (Recommended)

```bash
# Clone the repository
git clone https://github.com/Aicua/Unilang.git
cd Unilang

# Create build directory
mkdir build
cd build

# Generate build files
cmake .. -G "Visual Studio 17 2022" -A x64

# Build the project
cmake --build . --config Release

# The executable will be in build/bin/UniLang.exe
```

### Using Visual Studio

1. Open CMake project in Visual Studio 2022
2. Select `x64-Release` configuration
3. Build → Build All
4. Find executable in `out/build/x64-Release/bin/UniLang.exe`

## Usage

1. **Run the Application**: Double-click `UniLang.exe` (runs in system tray)
2. **Type Shortcuts**: In any application, type a LaTeX command like `\alpha`
3. **Get Results**: The shortcut is automatically replaced with `α`

### Example Shortcuts

| Type | Get | Type | Get |
|------|-----|------|-----|
| `\alpha` | α | `\beta` | β |
| `\sum` | ∑ | `\int` | ∫ |
| `\infty` | ∞ | `\pm` | ± |
| `\neq` | ≠ | `\leq` | ≤ |

See `config/shortcuts.json` for the complete list of 200+ shortcuts.

## Configuration

Edit `config/shortcuts.json` to add your own shortcuts:

```json
{
  "shortcuts": {
    "custom": {
      "\\myshortcut": "my replacement text"
    }
  }
}
```

## Contributing
We welcome contributions from the community! If you'd like to contribute to UniLang, please check out our [Contributing Guidelines](link-to-contributing-guidelines.md) for more information.

## License
UniLang is released under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Contact
For any inquiries or support, feel free to reach out:
- **Email**: support@unilang.org
- **GitHub**: [Aicua](https://github.com/Aicua)