#include "merc/paths.h"
#include "merc/windows-utils.h"
#include <shlobj.h>
#include <Windows.h>

namespace merc
{
    static std::filesystem::path getPathAndFree(PWSTR wide)
    {
        std::string buf(WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr) - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, buf.data(), buf.size(), nullptr, nullptr);
        CoTaskMemFree(wide);
        return { std::move(buf) };
    }
 
    std::filesystem::path getVstDefaultSystemDir()
    {
        PWSTR wide{};
        THROW_IF_FAILED(SHGetKnownFolderPath(FOLDERID_ProgramFilesCommon, 0, nullptr, &wide));
        return getPathAndFree(wide) / "VST3";
    }

    std::filesystem::path getVstDefaultUserDir()
    {
        PWSTR wide{};
        THROW_IF_FAILED(SHGetKnownFolderPath(FOLDERID_UserProgramFilesCommon, 0, nullptr, &wide));
        return getPathAndFree(wide) / "VST3";
    }

    std::filesystem::path getModuleDir()
    {
        TCHAR path[512];
        if (GetModuleFileName(getCurrentModule(), path, sizeof(path)) == 0)
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        return std::filesystem::path{ path }.parent_path();
    }

    std::filesystem::path getModuleResourceDir()
    {
        return getModuleDir().parent_path() / "Resources";
    }
}
