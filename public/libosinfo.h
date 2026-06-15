#ifndef OSINFO_LIBOSINFO_H_
#define OSINFO_LIBOSINFO_H_

#ifndef OSINFO_STATIC_LIB
 #include <shlwapi.h>
#endif // OSINFO_STATIC_LIB

// Tiny helper for making code that can be used for both .dlls and static .libs.
#ifndef DLL_IMPORT
 #define DLL_IMPORT __declspec(dllimport)
#endif

#ifndef DLL_EXPORT
 #define DLL_EXPORT __declspec(dllexport)
#endif

// Use OSINFO_API for exported functions. When building the DLL, define
// OSINFO_DLL_EXPORTS to export symbols.
#if defined(OSINFO_DLL_EXPORTS) // For use by osinfo when building DLL
 #define OSINFO_API DLL_EXPORT
#elif defined(OSINFO_DLL_IMPORTS) // Only for use by consumers (of either statis lib or .dll).
 #define OSINFO_API DLL_IMPORT
#elif defined(OSINFO_STATIC_LIB) // For use by osinfo when building static library.
 #define OSINFO_API
#endif // OSINFO_DLL_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSINFO_STATIC_LIB
 /* DllGetVersion accepts pointer to DLLVERSIONINFO or DLLVERSIONINFO2.
  * The structure type is determined at runtime by the cbSize field. */
 // Standard version getter function for DLLs
 OSINFO_API HRESULT __cdecl DllGetVersion(DLLVERSIONINFO* pdvi);

 // Use to init .dll, but use EnsureInitialized() for everything else.
 OSINFO_API bool __cdecl InitOsInfoDll();
 // Opposite of InitOsInfoDLL, but only to be called privately.
 OSINFO_API bool __cdecl DeInitOsInfoDLL();
#endif // !OSINFO_STATIC_LIB

// Ensures we are initialized.
OSINFO_API bool __cdecl EnsureInitialized();

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
OSINFO_API std::wstring const __cdecl GetOsInfoVersionW();

// Returns an unsigned long long representing the full NT build number.
// For example, for Windows 7 SP1, it would return 0x6011DB1.
OSINFO_API unsigned long long const __cdecl GetRawNTVer();

// Returns an unsigned long representing the first two major digits of
// the NT build number. For example, for Windows 7 SP1, it would return 0x601.
OSINFO_API unsigned long const __cdecl GetShortNTVer();

// Gets the service pack number as an int, returns 0 for RTM with no service pack
// and returns -1 if not initialized.
OSINFO_API int const __cdecl GetServicePackNumber();

// Gets the service pack as a human readable string.
OSINFO_API std::string const __cdecl GetServicePackA();

// Wide version of the above
OSINFO_API std::wstring const __cdecl GetServicePackW();

// USE THESE for the below IsAtLeast() function;
// Windows NT 4.0 (and Terminal Server)
inline constexpr unsigned long kWinNT4       = 0x0400L;
// Windows 2000 (and 2000 Server)
inline constexpr unsigned long kWin2000      = 0x0500L;
inline constexpr unsigned long kWin2K        = kWin2000;
inline constexpr unsigned long kWinSrv2000   = kWin2000;
// Windows XP
inline constexpr unsigned long kWinXP        = 0x0501L;
inline constexpr unsigned long kWinWhistler  = kWinXP;
// Windows XP x64/Server 2003 (and 2003 R2)
inline constexpr unsigned long kWinSrv2003   = 0x0502L;
inline constexpr unsigned long kWinSrv2003R2 = kWinSrv2003;
inline constexpr unsigned long kWin2003      = kWinSrv2003;
inline constexpr unsigned long kWin2003R2    = kWinSrv2003;
inline constexpr unsigned long kWinXP64      = kWinSrv2003;
inline constexpr unsigned long kWinXPx64     = kWinSrv2003;
// Windows Vista/Server 2008
inline constexpr unsigned long kWinVista     = 0x0600L;
inline constexpr unsigned long kWinSrv2008   = kWinVista;
inline constexpr unsigned long kWin2008      = kWinVista;
inline constexpr unsigned long kWinLonghorn  = kWinVista;
// Windows 7/Server 2008 R2/Home Server 2011
inline constexpr unsigned long kWin7         = 0x0601L;
inline constexpr unsigned long kWinSrv2008R2 = kWin7;
inline constexpr unsigned long kWin2008R2    = kWin7;
inline constexpr unsigned long kWin2011      = kWin2008R2;
// Windows 8/Server 2012
inline constexpr unsigned long kWin8         = 0x0602L;
inline constexpr unsigned long kWinSrv2012   = kWin8;
inline constexpr unsigned long kWin2012      = kWin8;
// Windows 8.1/Server 2012 R2
inline constexpr unsigned long kWin81        = 0x0603L;
inline constexpr unsigned long kWinSrv2012R2 = kWin81;
inline constexpr unsigned long kWin2012R2    = kWin81;
inline constexpr unsigned long kWinBlue      = kWin2012R2;
// Windows 10
inline constexpr unsigned long kWin10        = 0x0A00L;
// Windows 11
inline constexpr unsigned long kWin11        = 0x0A00L; // Win11 isn't higher

// Bool to check if Windows version is exactly the one passed in check_ver
OSINFO_API const bool __cdecl IsWin(const unsigned long check_ver);

// Checks if Windows version is at least the one passed (or higher) in check_ver
OSINFO_API const bool __cdecl IsAtLeast(const unsigned long check_ver);

// Checks if Windows version is at most the one passed (or less) in check_ver
OSINFO_API const bool __cdecl IsAtMost(const unsigned long check_ver);

// Checks if Windows version is newer than the one passed in check_ver
OSINFO_API const bool __cdecl IsWinNewerThan(const unsigned long check_ver);

// Checks if Windows version is older than the one passed in check_ver
OSINFO_API const bool __cdecl IsWinOlderThan(const unsigned long check_ver);

// Returns if we are running under WoW64, i.e. 32 bit app on 64 bit Windows.
OSINFO_API const bool __cdecl IsWoW64();

/* Simple bool functions for getting if is a specific version */
OSINFO_API const bool __cdecl IsWinNT4();
OSINFO_API const bool __cdecl IsWin2K();
OSINFO_API const bool __cdecl IsWinXP();
OSINFO_API const bool __cdecl IsWin2003();
OSINFO_API const bool __cdecl IsWinVista();
OSINFO_API const bool __cdecl IsWin7();
OSINFO_API const bool __cdecl IsWin8();
OSINFO_API const bool __cdecl IsWin8_1();
OSINFO_API const bool __cdecl IsWin10();
OSINFO_API const bool __cdecl IsWin11();

#ifndef OSINFO_STATIC_LIB
 // Pre-defined Typedefs for dynamically accessing
 // osinfo.dll functions using GetProcAddress (you could make up your own)
 typedef std::string (*pGetOSNameA)();

 typedef std::wstring (*pGetOSNameW)();

 typedef std::string (*pGetWinVersionA)();

 typedef std::wstring (*pGetWinVersionW)();

 typedef std::wstring (*pGetOsInfoVersionW)();

 typedef unsigned long long (*pGetRawNTVer)();

 typedef unsigned long (*pGetShortNTVer)();

 typedef BOOL (*pIsWin)(const ULONG);
 typedef BOOL (*pIsAtLeast)(const ULONG);
 typedef BOOL (*pIsWinNewerThan)(const ULONG);
 typedef BOOL (*pIsWinOlderThan)(const ULONG);
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
 typedef BOOL (*pIsWoW64)();
#endif // !OSINFO_STATIC_LIB

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OSINFO_LIBOSINFO_H_
