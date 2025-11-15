package com.aicua.unilang

import android.content.Context
import com.google.gson.Gson
import com.google.gson.JsonObject
import java.io.InputStreamReader

/**
 * Manages the shortcuts dictionary for text replacement
 * Ported from C++ shortcuts_dict.cpp
 *
 * Example: "\alpha" -> "α"
 */
class ShortcutsDict(private val context: Context) {
    private val shortcuts = mutableMapOf<String, String>()
    private var loaded = false

    /**
     * Load shortcuts from JSON asset file
     * @param assetPath Path to shortcuts.json in assets folder
     * @return true if loaded successfully
     */
    fun loadFromAssets(assetPath: String = "shortcuts.json"): Boolean {
        try {
            val inputStream = context.assets.open(assetPath)
            val reader = InputStreamReader(inputStream)
            val gson = Gson()
            val jsonObject = gson.fromJson(reader, JsonObject::class.java)

            shortcuts.clear()

            // Load all shortcuts from nested structure
            if (jsonObject.has("shortcuts")) {
                val shortcutsObj = jsonObject.getAsJsonObject("shortcuts")

                // Iterate through all categories
                for ((categoryKey, categoryValue) in shortcutsObj.entrySet()) {
                    if (categoryKey == "_comment") continue

                    if (categoryValue.isJsonObject) {
                        val categoryObj = categoryValue.asJsonObject
                        // Load shortcuts from this category
                        for ((shortcut, replacement) in categoryObj.entrySet()) {
                            shortcuts[shortcut] = replacement.asString
                        }
                    }
                }
            }

            loaded = true
            reader.close()
            return true

        } catch (e: Exception) {
            e.printStackTrace()
            return false
        }
    }

    /**
     * Find replacement for a shortcut
     * @param shortcut The shortcut to look up (e.g., "\\alpha")
     * @return The Unicode replacement if found, null otherwise
     */
    fun findReplacement(shortcut: String): String? {
        return shortcuts[shortcut]
    }

    /**
     * Get all shortcuts (for UI display)
     * @return Map of shortcuts to replacements
     */
    fun getAllShortcuts(): Map<String, String> {
        return shortcuts.toMap()
    }

    /**
     * Check if shortcuts dictionary is loaded
     */
    fun isLoaded(): Boolean = loaded

    /**
     * Get total number of shortcuts
     */
    fun getShortcutCount(): Int = shortcuts.size
}
