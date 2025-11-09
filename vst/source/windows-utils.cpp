#include "merc/windows-utils.h"

namespace merc
{
    HMODULE getCurrentModule()
    {
        HMODULE hm = NULL;
        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCWSTR)&getCurrentModule, &hm) == 0)
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        return hm;
    }

    std::wstring toUtf16(const std::string& utf8)
    {
        if (utf8.size() == 0) return {};
        const auto size{ MultiByteToWideChar(CP_UTF8, 0, utf8.data(), utf8.size(), nullptr, 0)};
        if (size == 0)
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        std::wstring ret(size, L'\0');
        if (MultiByteToWideChar(CP_UTF8, 0, utf8.data(), utf8.size(), ret.data(), size) != size)
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        return ret;
    }

    std::string toUtf8(const std::wstring& utf16)
    {
        if (utf16.size() == 0) return {};
        const auto size{ WideCharToMultiByte(CP_UTF8, 0, utf16.data(), utf16.size(), nullptr, 0, nullptr, nullptr)};
        if (size == 0)
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        std::string ret(size, '\0');
        if (WideCharToMultiByte(CP_UTF8, 0, utf16.data(), utf16.size(), ret.data(), size, nullptr, nullptr) != size)
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        return ret;
    }
}
