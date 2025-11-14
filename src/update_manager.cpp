#include "update_manager.h"
#include <nlohmann/json.hpp>
#include <wininet.h>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "wininet.lib")

using json = nlohmann::json;

namespace UniLang {

UpdateManager::UpdateManager() {
}

UpdateManager::~UpdateManager() {
}

std::string UpdateManager::HttpGet(const std::string& url) {
    m_last_error.clear();

    // Initialize WinINet
    HINTERNET hInternet = InternetOpenA(
        "UniLang/1.0",
        INTERNET_OPEN_TYPE_DIRECT,
        nullptr,
        nullptr,
        0
    );

    if (!hInternet) {
        m_last_error = "Failed to initialize WinINet";
        return "";
    }

    // Open URL
    HINTERNET hUrl = InternetOpenUrlA(
        hInternet,
        url.c_str(),
        nullptr,
        0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
        0
    );

    if (!hUrl) {
        m_last_error = "Failed to open URL: " + url;
        InternetCloseHandle(hInternet);
        return "";
    }

    // Read response
    std::string response;
    char buffer[4096];
    DWORD bytesRead = 0;

    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    // Cleanup
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    return response;
}

bool UpdateManager::IsVersionNewer(const std::string& version1, const std::string& version2) {
    // Parse semantic versions (e.g., "1.0.1" or "v1.0.1")
    auto parse_version = [](const std::string& v) -> std::tuple<int, int, int> {
        std::string ver = v;
        // Remove 'v' prefix if present
        if (!ver.empty() && ver[0] == 'v') {
            ver = ver.substr(1);
        }

        int major = 0, minor = 0, patch = 0;
        std::stringstream ss(ver);
        char dot;

        ss >> major;
        if (ss >> dot >> minor) {
            if (ss >> dot >> patch) {
                // Successfully parsed all three components
            }
        }

        return std::make_tuple(major, minor, patch);
    };

    auto [major1, minor1, patch1] = parse_version(version1);
    auto [major2, minor2, patch2] = parse_version(version2);

    // Compare versions
    if (major2 > major1) return true;
    if (major2 < major1) return false;

    if (minor2 > minor1) return true;
    if (minor2 < minor1) return false;

    if (patch2 > patch1) return true;

    return false;
}

UpdateManager::VersionInfo UpdateManager::ParseGitHubResponse(
    const std::string& json_response,
    const std::string& current_version
) {
    VersionInfo info;

    try {
        json j = json::parse(json_response);

        // Get version from tag_name (e.g., "v1.0.1")
        if (j.contains("tag_name")) {
            info.version = j["tag_name"].get<std::string>();
        }

        // Get release notes
        if (j.contains("body")) {
            info.release_notes = j["body"].get<std::string>();
        }

        // Find download URLs in assets
        if (j.contains("assets") && j["assets"].is_array()) {
            for (const auto& asset : j["assets"]) {
                if (!asset.contains("name") || !asset.contains("browser_download_url")) {
                    continue;
                }

                std::string name = asset["name"].get<std::string>();
                std::string download_url = asset["browser_download_url"].get<std::string>();

                // Look for .exe file
                if (name.find(".exe") != std::string::npos) {
                    info.download_url = download_url;
                }

                // Look for shortcuts.json
                if (name == "shortcuts.json") {
                    info.shortcuts_url = download_url;
                }
            }
        }

        // Check if newer
        info.is_newer = IsVersionNewer(current_version, info.version);

    } catch (const std::exception& e) {
        m_last_error = "Failed to parse GitHub response: " + std::string(e.what());
    }

    return info;
}

UpdateManager::VersionInfo UpdateManager::CheckForUpdates(
    const std::string& repo_owner,
    const std::string& repo_name,
    const std::string& current_version
) {
    VersionInfo info;

    // Build GitHub API URL
    std::string api_url = "https://api.github.com/repos/"
                        + repo_owner + "/" + repo_name
                        + "/releases/latest";

    // Make HTTP request
    std::string response = HttpGet(api_url);

    if (response.empty()) {
        // Error already set in HttpGet
        return info;
    }

    // Parse response
    info = ParseGitHubResponse(response, current_version);

    return info;
}

} // namespace UniLang
