#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <iostream>
#include <csignal>
#include <atomic>
#include "../../shortcuts_dict.h"
#include "../../pattern_matcher.h"
#include "../platform_interface.h"

namespace {
    std::atomic<bool> g_running{true};

    void signal_handler(int signal) {
        if (signal == SIGINT || signal == SIGTERM) {
            std::cout << "\nShutting down UniLang..." << std::endl;
            g_running = false;
            CFRunLoopStop(CFRunLoopGetCurrent());
        }
    }
}

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) NSStatusItem* statusItem;
@property UniLang::Platform::IKeyboardHook* keyboardHook;
@property UniLang::Platform::ITextInput* textInput;
@property UniLang::ShortcutsDict* shortcutsDict;
@property UniLang::PatternMatcher* patternMatcher;
@property std::string currentPattern;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    std::cout << "========================================" << std::endl;
    std::cout << "       UniLang - Universal Input       " << std::endl;
    std::cout << "   LaTeX-style Math & Symbol Shortcuts  " << std::endl;
    std::cout << "          macOS Version (Beta)          " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Create shortcuts dictionary
    self.shortcutsDict = new UniLang::ShortcutsDict();
    self.patternMatcher = new UniLang::PatternMatcher();

    // Load shortcuts from config file
    NSString* executablePath = [[NSBundle mainBundle] executablePath];
    NSString* executableDir = [executablePath stringByDeletingLastPathComponent];
    NSString* configPath = [executableDir stringByAppendingPathComponent:@"config/shortcuts.json"];

    if (!self.shortcutsDict->LoadFromFile([configPath UTF8String])) {
        // Try alternative path
        NSString* altConfigPath = [[NSBundle mainBundle] pathForResource:@"shortcuts" ofType:@"json"];
        if (altConfigPath) {
            self.shortcutsDict->LoadFromFile([altConfigPath UTF8String]);
        } else {
            std::cerr << "Warning: Could not load shortcuts configuration" << std::endl;
        }
    }

    std::cout << "Loaded " << self.shortcutsDict->GetShortcutCount() << " shortcuts" << std::endl;

    // Create status bar icon
    self.statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    self.statusItem.button.title = @"𝝰"; // Greek alpha symbol

    // Create menu
    NSMenu* menu = [[NSMenu alloc] init];
    [menu addItemWithTitle:@"UniLang v1.2.2" action:nil keyEquivalent:@""];
    [menu addItem:[NSMenuItem separatorItem]];
    [menu addItemWithTitle:@"About" action:@selector(showAbout:) keyEquivalent:@""];
    [menu addItemWithTitle:@"Quit UniLang" action:@selector(terminate:) keyEquivalent:@"q"];
    self.statusItem.menu = menu;

    // Create text input
    self.textInput = UniLang::Platform::PlatformFactory::CreateTextInput();

    // Create and install keyboard hook
    self.keyboardHook = UniLang::Platform::PlatformFactory::CreateKeyboardHook();

    if (self.keyboardHook) {
        bool success = self.keyboardHook->Install([self](uint32_t vkCode, bool isKeyDown) -> bool {
            return [self handleKeyEvent:vkCode isKeyDown:isKeyDown];
        });

        if (success) {
            std::cout << "✓ Keyboard hook installed" << std::endl;
            std::cout << "✓ UniLang is now active!" << std::endl;
            std::cout << std::endl;
            std::cout << "Try typing shortcuts like \\alpha or \\sum" << std::endl;
            std::cout << "Press Cmd+Q to quit" << std::endl;
        } else {
            std::cerr << "✗ Failed to install keyboard hook" << std::endl;
            std::cerr << "Please grant Accessibility permissions:" << std::endl;
            std::cerr << "System Preferences > Security & Privacy > Privacy > Accessibility" << std::endl;
        }
    }
}

- (bool)handleKeyEvent:(uint32_t)vkCode isKeyDown:(bool)isKeyDown {
    if (!isKeyDown) {
        return false; // Don't block key up events
    }

    // Convert key code to character (simplified)
    // In a full implementation, we'd need proper key code to character mapping

    // For now, just demonstrate the pattern matching concept
    char keyChar = 0;

    // Map common keys (this is simplified - real implementation would be more comprehensive)
    if (vkCode >= 0 && vkCode <= 127) {
        // TODO: Proper key code to character conversion
        // This would need to handle the macOS keyboard layout
    }

    // Pattern matching logic would go here
    // For now, just pass through all events
    return false; // Don't block
}

- (void)showAbout:(id)sender {
    NSAlert* alert = [[NSAlert alloc] init];
    alert.messageText = @"UniLang v1.2.2";
    alert.informativeText = @"Universal Language & Text Replacement Tool\n\n"
                            @"LaTeX-style shortcuts for typing mathematical symbols, "
                            @"Greek letters, and special characters.\n\n"
                            @"© 2024 Aicua";
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

- (void)terminate:(id)sender {
    std::cout << "Shutting down UniLang..." << std::endl;

    if (self.keyboardHook) {
        self.keyboardHook->Uninstall();
        delete self.keyboardHook;
        self.keyboardHook = nullptr;
    }

    if (self.textInput) {
        delete self.textInput;
        self.textInput = nullptr;
    }

    if (self.shortcutsDict) {
        delete self.shortcutsDict;
        self.shortcutsDict = nullptr;
    }

    if (self.patternMatcher) {
        delete self.patternMatcher;
        self.patternMatcher = nullptr;
    }

    [NSApp terminate:nil];
}

- (void)applicationWillTerminate:(NSNotification *)notification {
    std::cout << "UniLang terminated." << std::endl;
}

@end

int main(int argc, char* argv[]) {
    @autoreleasepool {
        // Set up signal handlers
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Create application
        NSApplication* app = [NSApplication sharedApplication];

        // Create and set delegate
        AppDelegate* delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];

        // Set activation policy (menu bar app)
        [app setActivationPolicy:NSApplicationActivationPolicyAccessory];

        // Run the application
        [app run];

        return 0;
    }
}
