#ifndef OSINFO_DLL_OS_INFO_DLL_H_
#define OSINFO_DLL_OS_INFO_DLL_H_

#include "export.h"
#include <shlwapi.h>

#ifdef __cplusplus
extern "C" {
#endif

// DLL equivalent of WinMain
OSINFO_API BOOL __cdecl DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved);

/* DllGetVersion accepts pointer to DLLVERSIONINFO or DLLVERSIONINFO2.
 * The structure type is determined at runtime by the cbSize field. */
// Standard version getter function for DLLs
OSINFO_API HRESULT __cdecl DllGetVersion(DLLVERSIONINFO* pdvi);

// Call to init info, but to use helper functions below to get values.
OSINFO_API const bool __cdecl InitOsInfoDll();

// Main function that gets the Windows version number.
// Called by InitOSInfoDLl and as needed in other functions to grab the version.
OSINFO_API bool __cdecl GetWinNTVersion();

// Returns human readable string like "Windows 7 Professional Service Pack 1"
OSINFO_API std::string const __cdecl GetOSNameA();

// Wide version of the above, recommended to use this version
OSINFO_API std::wstring const __cdecl GetOSNameW();

// Returns a string containing the human readable funn NT build number.
// For example, for Windows XP SP3, it would return "5.1.2600".
OSINFO_API std::string const __cdecl GetWinVersionA();

// Wide version of the above, recommended to use wchar_t
OSINFO_API std::wstring const __cdecl GetWinVersionW();

// Returns the version string of the dll itself.
OSINFO_API std::wstring const __cdecl GetOsInfoDllVersionW();

// Returns an unsigned long long representing the full NT build number.
// For example, for Windows 7 SP1, it would return 0x6011DB1.
OSINFO_API unsigned long long const __cdecl GetRawNTVer();

// Returns an unsigned long representing the first two major digits of
// the NT build number. For example, for Windows 7 SP1, it would return 0x601.
OSINFO_API unsigned long const __cdecl GetShortNTVer();

// USE THESE for the below IsAtLeast() function;
inline constexpr unsigned long kWinNT4 = 0x0400L;
inline constexpr unsigned long kWin2000 = 0x0500L;
inline constexpr unsigned long kWin2K = kWin2000;
inline constexpr unsigned long kWinXP = 0x0501L;
inline constexpr unsigned long kWinSrv2003 = 0x0502L;
inline constexpr unsigned long kWinXPx64 = kWinSrv2003;
inline constexpr unsigned long kWinVista = 0x0600L;
inline constexpr unsigned long kWinSrv2008 = kWinVista;
inline constexpr unsigned long kWin7 = 0x0601L;
inline constexpr unsigned long kWinSrv2008R2 = kWin7;
inline constexpr unsigned long kWin8 = 0x0602L;
inline constexpr unsigned long kWinSrv2012 = kWin8;
inline constexpr unsigned long kWin81 = 0x0603L;
inline constexpr unsigned long kWinSrv2012R2 = kWin81;
inline constexpr unsigned long kWinBlue = kWin81;
inline constexpr unsigned long kWin10 = 0x0A00L;
inline constexpr unsigned long kWin11 = kWin10;

// Bool to check if Windows version is exactly the one passed in check_ver
OSINFO_API const bool IsWin(const unsigned long check_ver);

// Checks if Windows version is at least the one passed (or higher) in check_ver
OSINFO_API const bool IsAtLeast(const unsigned long check_ver);

// Checks if Windows version is newer than the one passed in check_ver
OSINFO_API const bool IsWinNewerThan(const unsigned long check_ver);

// Checks if Windows version is older than the one passed in check_ver
OSINFO_API const bool IsWinOlderThan(const unsigned long check_ver);

/* Simple bool functions for getting if is a specific version */
OSINFO_API const bool IsWinNT4();
OSINFO_API const bool IsWin2K();
OSINFO_API const bool IsWinXP();
OSINFO_API const bool IsWin2003();
OSINFO_API const bool IsWinVista();
OSINFO_API const bool IsWin7();
OSINFO_API const bool IsWin8();
OSINFO_API const bool IsWin8_1();
OSINFO_API const bool IsWin10();
OSINFO_API const bool IsWin11();

// Pre-defined Typedefs for dynamically accessing
// osinfo.dll functions using GetProcAddress
typedef std::string (*pGetOSNameA)();

typedef std::wstring (*pGetOSNameW)();

typedef std::string (*pGetWinVersionA)();

typedef std::wstring (*pGetWinVersionW)();

typedef std::wstring (*pGetOsInfoDllVersionW)();

typedef unsigned long long (*pGetRawNTVer)();

typedef unsigned long (*pGetShortNTVer)();

typedef BOOL (*pIsAtLeast)();
typedef BOOL (*pIsWinNT4)();
typedef BOOL (*pIsWin2K)();
typedef BOOL (*pIsWinXP)();
typedef BOOL (*pIsWin2003)();
typedef BOOL (*pIsWinVista)();
typedef BOOL (*pIsWin7)();
typedef BOOL (*pIsWin8)();
typedef BOOL (*pIsWin8_1)();
typedef BOOL (*pIsWin10)();
typedef BOOL (*pIsWin11)();

#ifdef __cplusplus
}
#endif

#endif // OSINFO_DLL_OS_INFO_DLL_H_
