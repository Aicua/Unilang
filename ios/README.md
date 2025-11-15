# UniLang iOS - Keyboard Extension

LaTeX-style Math & Symbol Shortcuts keyboard for iOS.

## ⚠️ Status: Core Logic Only

This directory contains the **core Swift logic** ported from desktop C++:
- ✅ `ShortcutsDict.swift` - Shortcuts dictionary
- ✅ `PatternMatcher.swift` - Pattern detection

**What's NOT included yet:**
- ❌ Full iOS app project (.xcodeproj)
- ❌ Keyboard extension implementation
- ❌ UI components
- ❌ App Store assets

## Why?

Building a complete iOS app requires:
1. **macOS** with Xcode installed
2. **Apple Developer account** ($99/year)
3. **Signing certificates** for building
4. **Keyboard extension target** setup
5. **App Store submission** and review

This is a **proof-of-concept** showing the core logic compiles on iOS.

## GitHub Actions

The workflow builds the Swift package to verify:
- ✅ Swift syntax is correct
- ✅ Code compiles on iOS
- ✅ Logic is portable from C++

## Requirements

- iOS 14.0 or later
- Swift 5.9 or later
- Xcode 15.0 or later (for development)

## Building Locally

### Using Swift Package Manager

```bash
cd ios
swift build
```

### What This Proves

- ✅ Core logic (shortcuts dictionary, pattern matching) works
- ✅ Can be built on macOS
- ✅ Ready to integrate into full iOS app

## Next Steps

To create a complete iOS keyboard app, you would need to:

1. **Create Xcode Project**
   ```
   Xcode → New Project → iOS App
   Add Custom Keyboard Extension
   ```

2. **Integrate Core Logic**
   ```swift
   // Use ShortcutsDict and PatternMatcher
   let dict = ShortcutsDict()
   let matcher = PatternMatcher()
   ```

3. **Implement Keyboard Extension**
   ```swift
   class UniLangKeyboard: UIInputViewController {
       // Implement keyboard UI and logic
   }
   ```

4. **Configure App**
   - Info.plist settings
   - Keyboard permissions
   - App icon and assets

5. **Sign and Build**
   - Apple Developer account
   - Provisioning profiles
   - Distribution certificates

6. **Submit to App Store**
   - Screenshots
   - App description
   - Privacy policy
   - Wait for review (1-7 days)

## Comparison

| Component | Desktop (C++) | Android (Kotlin) | iOS (Swift) |
|-----------|--------------|------------------|-------------|
| **ShortcutsDict** | ✅ Original | ✅ Ported | ✅ Ported |
| **PatternMatcher** | ✅ Original | ✅ Ported | ✅ Ported |
| **Full App** | ✅ Windows/Mac/Linux | ✅ Android APK | ⏳ Needs Xcode project |

## Cost Analysis

| Platform | Development | Distribution | Total Year 1 |
|----------|-------------|--------------|--------------|
| **Desktop** | FREE | FREE | $0 |
| **Android** | FREE | $25 (one-time) | $25 |
| **iOS** | Mac required | $99/year | $99+ |

## License

MIT License - See LICENSE file for details

## Notes

- This is a minimal Swift package for testing
- Full keyboard extension requires Xcode project
- Cannot be installed on iPhone without proper app structure
- GitHub Actions validates the Swift code compiles
