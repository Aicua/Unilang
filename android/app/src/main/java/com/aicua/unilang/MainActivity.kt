package com.aicua.unilang

import android.content.Intent
import android.os.Bundle
import android.provider.Settings
import android.view.View
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

/**
 * Main Activity - Settings and About screen
 */
class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Setup info text
        val infoText = findViewById<TextView>(R.id.infoText)
        infoText.text = getString(R.string.setup_instructions)

        // Enable keyboard button
        val enableButton = findViewById<Button>(R.id.enableButton)
        enableButton.setOnClickListener {
            startActivity(Intent(Settings.ACTION_INPUT_METHOD_SETTINGS))
        }

        // About text
        val aboutText = findViewById<TextView>(R.id.aboutText)
        aboutText.text = getString(R.string.about_description)

        // Load shortcuts count
        val shortcutsDict = ShortcutsDict(this)
        if (shortcutsDict.loadFromAssets()) {
            val countText = findViewById<TextView>(R.id.shortcutsCount)
            countText.text = "Loaded ${shortcutsDict.getShortcutCount()} shortcuts"
        }
    }
}
