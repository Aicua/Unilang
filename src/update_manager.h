#pragma once

#include <string>
#include <Windows.h>

namespace UniLang {

/**
 * @brief Manages version checking and update availability
 *
 * Responsibilities:
 * - Query GitHub API for latest release
 * - Compare versions
 * - Parse release information (version, download URL, release notes)
 */
class UpdateManager {
public:
    struct VersionInfo {
        std::string version;           // e.g., "1.0.1"
        std::string download_url;      // URL to .exe file
        std::string shortcuts_url;     // URL to shortcuts.json
        std::string release_notes;     // Description from GitHub release
        bool is_newer = false;         // True if newer than current version
    };

    UpdateManager();
    ~UpdateManager();

    /**
     * @brief Check GitHub API for latest release
     * @param repo_owner GitHub repository owner (e.g., "aicua")
     * @param repo_name GitHub repository name (e.g., "unilang")
     * @param current_version Current version to compare against
     * @return VersionInfo with latest release details
     */
    VersionInfo CheckForUpdates(
        const std::string& repo_owner,
        const std::string& repo_name,
        const std::string& current_version
    );

    /**
     * @brief Get last error message
     */
    std::string GetLastError() const { return m_last_error; }

private:
    /**
     * @brief Make HTTP GET request to GitHub API
     * @param url Full API URL
     * @return JSON response as string
     */
    std::string HttpGet(const std::string& url);

    /**
     * @brief Compare two semantic versions (e.g., "1.0.1" vs "1.0.2")
     * @return true if version1 < version2
     */
    bool IsVersionNewer(const std::string& version1, const std::string& version2);

    /**
     * @brief Parse GitHub API response and extract version info
     */
    VersionInfo ParseGitHubResponse(const std::string& json_response, const std::string& current_version);

private:
    std::string m_last_error;
};

} // namespace UniLang
