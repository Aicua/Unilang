package com.aicua.unilang

import android.inputmethodservice.InputMethodService
import android.inputmethodservice.Keyboard
import android.inputmethodservice.KeyboardView
import android.view.KeyEvent
import android.view.View
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputConnection

/**
 * UniLang Input Method Editor (Keyboard)
 * Implements LaTeX-style shortcuts for Android
 */
class UniLangIME : InputMethodService(), KeyboardView.OnKeyboardActionListener {

    private var keyboardView: KeyboardView? = null
    private var keyboard: Keyboard? = null

    private lateinit var shortcutsDict: ShortcutsDict
    private lateinit var patternMatcher: PatternMatcher

    override fun onCreate() {
        super.onCreate()

        // Initialize shortcuts dictionary
        shortcutsDict = ShortcutsDict(this)
        shortcutsDict.loadFromAssets("shortcuts.json")

        // Initialize pattern matcher
        patternMatcher = PatternMatcher()
    }

    override fun onCreateInputView(): View {
        keyboardView = layoutInflater.inflate(R.layout.keyboard_view, null) as KeyboardView
        keyboard = Keyboard(this, R.xml.qwerty)
        keyboardView?.keyboard = keyboard
        keyboardView?.setOnKeyboardActionListener(this)
        return keyboardView!!
    }

    override fun onStartInput(attribute: EditorInfo?, restarting: Boolean) {
        super.onStartInput(attribute, restarting)
        patternMatcher.reset()
    }

    override fun onKey(primaryCode: Int, keyCodes: IntArray?) {
        val ic = currentInputConnection ?: return

        when (primaryCode) {
            Keyboard.KEYCODE_DELETE -> {
                ic.deleteSurroundingText(1, 0)
            }
            Keyboard.KEYCODE_DONE -> {
                ic.sendKeyEvent(KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_ENTER))
            }
            else -> {
                val char = primaryCode.toChar()

                // Add character to pattern matcher
                val shouldContinue = patternMatcher.addChar(char)

                if (shouldContinue && patternMatcher.hasPattern()) {
                    // Check if current pattern matches a shortcut
                    val pattern = patternMatcher.getCurrentPattern()
                    val replacement = shortcutsDict.findReplacement(pattern)

                    if (replacement != null) {
                        // Found a match! Replace the pattern
                        val patternLength = patternMatcher.getPatternLength()

                        // Delete the pattern
                        ic.deleteSurroundingText(patternLength, 0)

                        // Insert the replacement
                        ic.commitText(replacement, 1)

                        // Reset pattern matcher
                        patternMatcher.reset()
                    } else {
                        // No match yet, just type the character
                        ic.commitText(char.toString(), 1)
                    }
                } else {
                    // Not part of a pattern, just type the character
                    ic.commitText(char.toString(), 1)

                    if (!shouldContinue) {
                        patternMatcher.reset()
                    }
                }
            }
        }
    }

    override fun onText(text: CharSequence?) {
        val ic = currentInputConnection ?: return
        ic.commitText(text, 1)
    }

    override fun swipeLeft() {}
    override fun swipeRight() {}
    override fun swipeDown() {}
    override fun swipeUp() {}
    override fun onPress(primaryCode: Int) {}
    override fun onRelease(primaryCode: Int) {}
}
