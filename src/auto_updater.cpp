#include "auto_updater.h"
#include <urlmon.h>
#include <shlwapi.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "shlwapi.lib")

namespace UniLang {

// Callback for download progress
class DownloadCallback : public IBindStatusCallback {
public:
    DownloadCallback(AutoUpdater::ProgressCallback callback)
        : m_callback(callback), m_ref_count(1) {}

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown || riid == IID_IBindStatusCallback) {
            *ppv = static_cast<IBindStatusCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)() {
        return ++m_ref_count;
    }

    STDMETHOD_(ULONG, Release)() {
        ULONG count = --m_ref_count;
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // IBindStatusCallback methods
    STDMETHOD(OnStartBinding)(DWORD, IBinding*) { return S_OK; }
    STDMETHOD(GetPriority)(LONG*) { return E_NOTIMPL; }
    STDMETHOD(OnLowResource)(DWORD) { return S_OK; }
    STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG, LPCWSTR) {
        if (m_callback && ulProgressMax > 0) {
            int percentage = (int)((ulProgress * 100) / ulProgressMax);
            m_callback(percentage, "Downloading...");
        }
        return S_OK;
    }
    STDMETHOD(OnStopBinding)(HRESULT, LPCWSTR) { return S_OK; }
    STDMETHOD(GetBindInfo)(DWORD*, BINDINFO*) { return S_OK; }
    STDMETHOD(OnDataAvailable)(DWORD, DWORD, FORMATETC*, STGMEDIUM*) { return S_OK; }
    STDMETHOD(OnObjectAvailable)(REFIID, IUnknown*) { return S_OK; }

private:
    AutoUpdater::ProgressCallback m_callback;
    ULONG m_ref_count;
};

AutoUpdater::AutoUpdater() {
}

AutoUpdater::~AutoUpdater() {
}

std::string AutoUpdater::GetCurrentExePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    return std::string(buffer);
}

std::string AutoUpdater::GetAppDirectory() {
    std::string exe_path = GetCurrentExePath();
    size_t last_slash = exe_path.find_last_of("\\/");
    if (last_slash != std::string::npos) {
        return exe_path.substr(0, last_slash);
    }
    return "";
}

bool AutoUpdater::DownloadFile(
    const std::string& url,
    const std::string& destination,
    ProgressCallback callback
) {
    m_last_error.clear();

    // Convert to wide strings
    std::wstring url_w(url.begin(), url.end());
    std::wstring dest_w(destination.begin(), destination.end());

    // Create callback object
    DownloadCallback* dl_callback = new DownloadCallback(callback);

    // Download file
    HRESULT hr = URLDownloadToFileW(
        nullptr,
        url_w.c_str(),
        dest_w.c_str(),
        0,
        dl_callback
    );

    if (FAILED(hr)) {
        m_last_error = "Failed to download file from: " + url;
        return false;
    }

    return true;
}

std::string AutoUpdater::CreateUpdateBatchScript(
    const std::string& new_exe_path,
    const std::string& current_exe_path
) {
    std::string app_dir = GetAppDirectory();
    std::string batch_path = app_dir + "\\update.bat";

    std::ofstream batch_file(batch_path);
    if (!batch_file.is_open()) {
        m_last_error = "Failed to create update batch script";
        return "";
    }

    // Write batch script
    batch_file << "@echo off\n";
    batch_file << "echo Updating UniLang...\n";
    batch_file << "timeout /t 2 /nobreak >nul\n";  // Wait 2 seconds for app to close
    batch_file << "\n";
    batch_file << ":: Delete old executable\n";
    batch_file << "del /F /Q \"" << current_exe_path << "\"\n";
    batch_file << "\n";
    batch_file << ":: Rename new executable\n";
    batch_file << "move /Y \"" << new_exe_path << "\" \"" << current_exe_path << "\"\n";
    batch_file << "\n";
    batch_file << ":: Restart application\n";
    batch_file << "start \"\" \"" << current_exe_path << "\"\n";
    batch_file << "\n";
    batch_file << ":: Delete this batch script\n";
    batch_file << "del /F /Q \"%~f0\"\n";

    batch_file.close();

    return batch_path;
}

bool AutoUpdater::ExecuteUpdateAndExit(const std::string& batch_script_path) {
    // Execute batch script in hidden window
    SHELLEXECUTEINFOA sei = {};
    sei.cbSize = sizeof(SHELLEXECUTEINFOA);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = "open";
    sei.lpFile = batch_script_path.c_str();
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExA(&sei)) {
        m_last_error = "Failed to execute update script";
        return false;
    }

    return true;
}

bool AutoUpdater::DownloadAndInstall(
    const std::string& exe_url,
    const std::string& shortcuts_url,
    ProgressCallback callback
) {
    m_last_error.clear();
    m_progress_callback = callback;

    std::string app_dir = GetAppDirectory();
    std::string current_exe = GetCurrentExePath();

    // Download new executable
    if (callback) {
        callback(0, "Downloading new version...");
    }

    std::string new_exe_path = app_dir + "\\UniLang_new.exe";
    if (!DownloadFile(exe_url, new_exe_path, callback)) {
        return false;
    }

    // Download shortcuts.json if provided
    if (!shortcuts_url.empty()) {
        if (callback) {
            callback(50, "Downloading shortcuts...");
        }

        std::string shortcuts_path = app_dir + "\\config\\shortcuts.json";
        if (!DownloadFile(shortcuts_url, shortcuts_path, nullptr)) {
            // Non-critical, continue anyway
        }
    }

    if (callback) {
        callback(100, "Preparing to restart...");
    }

    // Create update batch script
    std::string batch_path = CreateUpdateBatchScript(new_exe_path, current_exe);
    if (batch_path.empty()) {
        return false;
    }

    // Execute batch script and exit
    if (!ExecuteUpdateAndExit(batch_path)) {
        return false;
    }

    // Exit current application
    ExitProcess(0);

    return true;
}

} // namespace UniLang
