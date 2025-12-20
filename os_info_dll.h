#ifndef OSINFO_DLL_OS_INFO_DLL_H_
#define OSINFO_DLL_OS_INFO_DLL_H_

#include "export.h"

extern "C"{

// Call to init info, but to use helper functions below to get values.
DLL_EXPORT bool __cdecl getWinNTVersion();

DLL_EXPORT std::string const __cdecl GetOSNameA();

DLL_EXPORT std::wstring const __cdecl GetOSNameW();

DLL_EXPORT std::string const __cdecl GetWinVersionA();

DLL_EXPORT std::wstring const __cdecl GetWinVersionW();

DLL_EXPORT std::wstring const __cdecl GetOsInfoDllVersionW();

DLL_EXPORT unsigned long long const __cdecl GetRawNTVer();

}

#endif // OSINFO_DLL_OS_INFO_DLL_H_
