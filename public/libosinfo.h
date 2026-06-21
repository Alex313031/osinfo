#ifndef OSINFO_LIBOSINFO_H_
#define OSINFO_LIBOSINFO_H_

// Master OSInfo library include file, this should be the only file consumers #include.

#include "cpuinfo.h"

#ifndef OSINFO_STATIC_LIB
 #include <shlwapi.h>
#endif // OSINFO_STATIC_LIB

// The DLL_IMPORT/DLL_EXPORT/OSINFO_API export macros are defined in lib_export.h which is included
// transitively via cpuinfo.h above. Consumers should not include either header directly.

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSINFO_STATIC_LIB
/* DllGetVersion accepts pointer to DLLVERSIONINFO or DLLVERSIONINFO2.
 * The structure type is determined at runtime by the cbSize field. */
// Standard version getter function for DLLs
OSINFO_API HRESULT __cdecl DllGetVersion(DLLVERSIONINFO* pdvi);
#endif // !OSINFO_STATIC_LIB

// Returns human readable string like "Windows 7 Professional Service Pack 1"
OSINFO_API std::string const __cdecl GetOSNameA();

// Wide version of the above, recommended to use this version
OSINFO_API std::wstring const __cdecl GetOSNameW();

// Returns a string containing the human readable full NT build number.
// For example, for Windows XP SP3, it would return "5.1.2600".
OSINFO_API std::string const __cdecl GetWinVersionA();

// Wide version of the above, recommended to use wchar_t
OSINFO_API std::wstring const __cdecl GetWinVersionW();

// Returns the version string of the library itself.
OSINFO_API std::string const __cdecl GetOsInfoVersionA();

// Wide version of the above, recommended to use wchar_t
OSINFO_API std::wstring const __cdecl GetOsInfoVersionW();

// Returns an unsigned long long representing the full NT build number.
// For example, for Windows 7 SP1, it would return 0x6011DB1.
// This is the standard, initialized version value (via RtlGetVersion) and is
// what most callers want. Use GetRealNTVer() when you don't trust the environment.
OSINFO_API unsigned long long const __cdecl GetReportedNTVer();

// Same packed format as GetReportedNTVer(), but always re-reads the version directly
// via ntdll's RtlGetNtVersionNumbers, which ignores application-compatibility
// shims/manifests and cannot be spoofed. Prefer this when you specifically need
// the true OS version regardless of how the process was launched.
OSINFO_API unsigned long long const __cdecl GetRealNTVer();

// Returns an unsigned long representing the first two major digits of
// the NT build number. For example, for Windows 7 SP1, it would return 0x601.
OSINFO_API unsigned long const __cdecl GetReportedShortNTVer();

// Same as GetReportedShortNTVer(), but from the unspoofable RtlGetNtVersionNumbers source.
OSINFO_API unsigned long const __cdecl GetRealShortNTVer();

// Gets the service pack number as an int, returns 0 for RTM with no service pack
// and returns -1 if not initialized.
OSINFO_API int const __cdecl GetServicePackNumber();

// Gets the service pack as a human readable string.
OSINFO_API std::string const __cdecl GetServicePackA();

// Wide version of the above
OSINFO_API std::wstring const __cdecl GetServicePackW();

// USE THESE for the below IsAtLeast() function;
// Windows NT 4.0 (and Terminal Server)
inline constexpr unsigned long kWinNT4 = 0x0400UL;
// Windows 2000 (and 2000 Server)
inline constexpr unsigned long kWin2000    = 0x0500UL;
inline constexpr unsigned long kWin2K      = kWin2000;
inline constexpr unsigned long kWinSrv2000 = kWin2000;
// Windows XP
inline constexpr unsigned long kWinXP       = 0x0501UL;
inline constexpr unsigned long kWinWhistler = kWinXP;
// Windows XP x64/Server 2003 (and 2003 R2)
inline constexpr unsigned long kWinSrv2003   = 0x0502UL;
inline constexpr unsigned long kWinSrv2003R2 = kWinSrv2003;
inline constexpr unsigned long kWin2003      = kWinSrv2003;
inline constexpr unsigned long kWin2003R2    = kWinSrv2003;
inline constexpr unsigned long kWinXP64      = kWinSrv2003;
inline constexpr unsigned long kWinXPx64     = kWinSrv2003;
// Windows Vista/Server 2008
inline constexpr unsigned long kWinVista    = 0x0600UL;
inline constexpr unsigned long kWinSrv2008  = kWinVista;
inline constexpr unsigned long kWin2008     = kWinVista;
inline constexpr unsigned long kWinLonghorn = kWinVista;
// Windows 7/Server 2008 R2/Home Server 2011
inline constexpr unsigned long kWin7         = 0x0601UL;
inline constexpr unsigned long kWinSrv2008R2 = kWin7;
inline constexpr unsigned long kWin2008R2    = kWin7;
inline constexpr unsigned long kWin2011      = kWin2008R2;
// Windows 8/Server 2012
inline constexpr unsigned long kWin8       = 0x0602UL;
inline constexpr unsigned long kWinSrv2012 = kWin8;
inline constexpr unsigned long kWin2012    = kWin8;
// Windows 8.1/Server 2012 R2
inline constexpr unsigned long kWin81        = 0x0603UL;
inline constexpr unsigned long kWinSrv2012R2 = kWin81;
inline constexpr unsigned long kWin2012R2    = kWin81;
inline constexpr unsigned long kWinBlue      = kWin2012R2;
// Windows 10
inline constexpr unsigned long kWin10 = 0x0A00UL;
// Windows 11. Win10 and Win11 both report NT version 10.0 (0x0A00), so this is a
// distinct, higher sentinel rather than the raw NT value. The version-comparison
// functions below (IsWin/IsAtLeast/etc.) special-case it: on a real Win11 (build
// >= 22000) they treat the running OS as kWin11, so it sorts above kWin10.
inline constexpr unsigned long kWin11 = 0x0B00UL;

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
// Pre-defined typedefs for dynamically resolving osinfo.dll functions with
// GetProcAddress (you could make up your own). The __cdecl matches the exported
// functions' calling convention - a mismatch corrupts the stack across the
// DLL boundary.
typedef HRESULT(__cdecl* pDllGetVersion)(DLLVERSIONINFO*);

typedef std::string(__cdecl* pGetOSNameA)();
typedef std::wstring(__cdecl* pGetOSNameW)();
typedef std::string(__cdecl* pGetWinVersionA)();
typedef std::wstring(__cdecl* pGetWinVersionW)();
typedef std::string(__cdecl* pGetOsInfoVersionA)();
typedef std::wstring(__cdecl* pGetOsInfoVersionW)();

typedef unsigned long long(__cdecl* pGetReportedNTVer)();
typedef unsigned long long(__cdecl* pGetRealNTVer)();
typedef unsigned long(__cdecl* pGetReportedShortNTVer)();
typedef unsigned long(__cdecl* pGetRealShortNTVer)();

typedef int(__cdecl* pGetServicePackNumber)();
typedef std::string(__cdecl* pGetServicePackA)();
typedef std::wstring(__cdecl* pGetServicePackW)();

typedef bool(__cdecl* pIsWin)(const ULONG);
typedef bool(__cdecl* pIsAtLeast)(const ULONG);
typedef bool(__cdecl* pIsAtMost)(const ULONG);
typedef bool(__cdecl* pIsWinNewerThan)(const ULONG);
typedef bool(__cdecl* pIsWinOlderThan)(const ULONG);

typedef bool(__cdecl* pIsWinNT4)();
typedef bool(__cdecl* pIsWin2K)();
typedef bool(__cdecl* pIsWinXP)();
typedef bool(__cdecl* pIsWin2003)();
typedef bool(__cdecl* pIsWinVista)();
typedef bool(__cdecl* pIsWin7)();
typedef bool(__cdecl* pIsWin8)();
typedef bool(__cdecl* pIsWin8_1)();
typedef bool(__cdecl* pIsWin10)();
typedef bool(__cdecl* pIsWin11)();
typedef bool(__cdecl* pIsWoW64)();

// CPU info (declared in cpuinfo.h).
typedef bool(__cdecl* pGetCPUInfo)(CPUID_INFO*);
typedef unsigned int(__cdecl* pNumCpuCores)();
typedef const wchar_t*(__cdecl* pCpuVendor)();
typedef const wchar_t*(__cdecl* pCpuModel)();
typedef bool(__cdecl* pIsCPU64BitCapable)();
typedef bool(__cdecl* pCanExecuteSSE)();
typedef bool(__cdecl* pCanExecuteAVX)();
typedef bool(__cdecl* pCanExecuteAVX512)();
typedef unsigned long(__cdecl* pCpuFreqAvg)();
typedef unsigned long(__cdecl* pCpuFreqMax)();
#endif // !OSINFO_STATIC_LIB

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OSINFO_LIBOSINFO_H_
