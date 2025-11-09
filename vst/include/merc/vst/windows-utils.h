#pragma once

#include <string>
#include <stdexcept>
#include <windows.h>

inline std::string hrToString(HRESULT hr, const char* msg)
{
    char s[64] = {};
    sprintf_s(s, ": HRESULT of 0x%08X", static_cast<UINT>(hr));
    return msg + std::string(s);
}

struct HrException : public std::runtime_error
{
    HrException(HRESULT hr, const char* msg)
        : std::runtime_error(hrToString(hr, msg))
    {}
};

#define THROW_IF_FAILED(hr) \
{ \
    HRESULT rrr{ hr }; \
    if (FAILED(rrr)) throw HrException(rrr, #hr); \
}

namespace merc
{
    HMODULE getCurrentModule();

    std::wstring toUtf16(const std::string& utf8);
    std::string toUtf8(const std::wstring& utf16);
}
