#ifndef OSINFO_DLL_OS_INFO_DLL_H_
#define OSINFO_DLL_OS_INFO_DLL_H_

#include "export.h"

extern "C"{

// Call to init info, but to use helper functions below to get values.
DLL_EXPORT const bool __cdecl InitOsInfoDll();

// Main function that gets the Windows version number.
// Called by InitOSInfoDLl and as needed in other functions to grab the version.
DLL_EXPORT bool __cdecl GetWinNTVersion();

// Returns human readable string like "Windows 7 Professional Service Pack 1"
DLL_EXPORT std::string const __cdecl GetOSNameA();

// Wide version of the above, recommended to use this version
DLL_EXPORT std::wstring const __cdecl GetOSNameW();

// Returns a string containing the human readable funn NT build number.
// For example, for Windows XP SP3, it would return "5.1.2600".
DLL_EXPORT std::string const __cdecl GetWinVersionA();

// Wide version of the above, recommended to use wchar_t
DLL_EXPORT std::wstring const __cdecl GetWinVersionW();

// Returns the version string of the dll itself.
DLL_EXPORT std::wstring const __cdecl GetOsInfoDllVersionW();

// Returns an unsigned long long representing the full NT build number.
// For example, for Windows 7 SP1, it would return 0x6011DB1.
DLL_EXPORT unsigned long long const __cdecl GetRawNTVer();

// Returns an unsigned long representing the first two major digits of
// the NT build number. For example, for Windows 7 SP1, it would return 0x601.
DLL_EXPORT unsigned long const __cdecl GetShortNTVer();

}

#endif // OSINFO_DLL_OS_INFO_DLL_H_
