#pragma once

#include <string>
#include <functional>
#include <Windows.h>

namespace UniLang {

/**
 * @brief Handles automatic downloading and installation of updates
 *
 * Responsibilities:
 * - Download executable and config files from URLs
 * - Replace current executable with new version
 * - Restart application after update
 */
class AutoUpdater {
public:
    using ProgressCallback = std::function<void(int percentage, const std::string& status)>;

    AutoUpdater();
    ~AutoUpdater();

    /**
     * @brief Download and install update
     * @param exe_url URL to new .exe file
     * @param shortcuts_url URL to new shortcuts.json (optional)
     * @param callback Progress callback (optional)
     * @return true if update initiated successfully
     */
    bool DownloadAndInstall(
        const std::string& exe_url,
        const std::string& shortcuts_url = "",
        ProgressCallback callback = nullptr
    );

    /**
     * @brief Get last error message
     */
    std::string GetLastError() const { return m_last_error; }

private:
    /**
     * @brief Download file from URL
     * @param url Source URL
     * @param destination Destination file path
     * @param callback Progress callback
     * @return true if successful
     */
    bool DownloadFile(
        const std::string& url,
        const std::string& destination,
        ProgressCallback callback
    );

    /**
     * @brief Create batch script to replace executable and restart
     * @param new_exe_path Path to new executable
     * @param current_exe_path Path to current executable
     * @return Path to created batch script
     */
    std::string CreateUpdateBatchScript(
        const std::string& new_exe_path,
        const std::string& current_exe_path
    );

    /**
     * @brief Execute batch script and exit application
     * @param batch_script_path Path to batch script
     * @return true if successful
     */
    bool ExecuteUpdateAndExit(const std::string& batch_script_path);

    /**
     * @brief Get current executable path
     */
    std::string GetCurrentExePath();

    /**
     * @brief Get application directory
     */
    std::string GetAppDirectory();

private:
    std::string m_last_error;
    ProgressCallback m_progress_callback;
};

} // namespace UniLang
