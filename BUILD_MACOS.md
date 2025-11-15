# Building UniLang on macOS

## Prerequisites

- macOS 10.15 (Catalina) or later
- Xcode Command Line Tools
- CMake 3.16 or higher

### Install Xcode Command Line Tools

```bash
xcode-select --install
```

### Install CMake

Using Homebrew:
```bash
brew install cmake
```

Or download from: https://cmake.org/download/

## Building

```bash
# Clone the repository
git clone https://github.com/Aicua/Unilang.git
cd Unilang

# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
cmake --build .

# The executable will be in build/bin/UniLang
./bin/UniLang
```

## Granting Accessibility Permissions

UniLang requires **Accessibility permissions** to intercept keyboard events system-wide.

**First time running:**

1. Run UniLang: `./bin/UniLang`
2. macOS will show a security dialog
3. Go to **System Preferences** → **Security & Privacy** → **Privacy** → **Accessibility**
4. Click the lock icon to make changes
5. Add UniLang to the list of allowed applications
6. Restart UniLang

**If you see "Failed to create event tap":**
- This means Accessibility permissions haven't been granted
- Follow the steps above to grant permissions

## Running UniLang

```bash
cd build/bin
./UniLang
```

UniLang will:
- Run in the background
- Show a menu bar icon (𝝰)
- Intercept keyboard shortcuts like `\alpha`, `\sum`, etc.

### Using UniLang

1. **Type a shortcut**: For example, type `\alpha`
2. **Get the symbol**: UniLang will replace it with `α`

### Menu Bar Options

Click the menu bar icon (𝝰) to access:
- **About**: Version information
- **Quit**: Exit UniLang

## Configuration

Shortcuts are loaded from `config/shortcuts.json`. To customize:

1. Copy `config/shortcuts.json` to your build directory:
   ```bash
   cp ../config/shortcuts.json bin/
   ```

2. Edit the JSON file to add your own shortcuts

## Troubleshooting

### "Failed to create event tap"
- Grant Accessibility permissions (see above)

### "Could not load shortcuts configuration"
- Make sure `config/shortcuts.json` exists
- Check the file path in the error message

### Build errors
- Make sure you have Xcode Command Line Tools installed
- Update CMake to version 3.16 or higher
- Check that you're using a compatible macOS version (10.15+)

## Uninstalling

```bash
# Remove the executable
rm -rf build

# Remove from Accessibility list
# System Preferences → Security & Privacy → Privacy → Accessibility
# Select UniLang and click the minus (-) button
```

## Development

### Project Structure

```
src/
├── platform/
│   └── macos/
│       ├── main_macos.mm              # Entry point
│       ├── keyboard_hook_macos.h/mm   # Keyboard interception
│       ├── text_input_macos.h/mm      # Text injection
│       └── platform_factory_macos.mm  # Factory pattern
```

### macOS APIs Used

- **CGEventTap**: Intercepts keyboard events system-wide
- **CGEventPost**: Simulates keyboard input
- **NSStatusBar**: Menu bar icon
- **NSApplication**: macOS application lifecycle

## License

MIT License - See LICENSE file for details
