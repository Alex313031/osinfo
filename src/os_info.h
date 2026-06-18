#ifndef OSINFO_OS_INFO_H_
#define OSINFO_OS_INFO_H_

#include "framework.h"

// Manual relative include
#include "../public/libosinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL(WINAPI* IS_WOW64_PROCESS_)(HANDLE hProcess, PBOOL Wow64Process);

// These can be spoofed
extern unsigned long WinVer;
extern unsigned long long WinVerFull;
// These can't be spoofed
extern unsigned long RealWinVer;
extern unsigned long long RealWinVerFull;

// For checking specific versions quickly
extern bool is_winnt;
extern bool is_win2k;
extern bool is_winxp;
extern bool is_win03;
extern bool is_win06;
extern bool is_win7;
extern bool is_win8;
extern bool is_win81;
extern bool is_win10;
extern bool is_win11;

// Spoofable
extern ULONG NT_MAJOR;
extern ULONG NT_MINOR;
extern ULONG NT_BUILD;
inline constexpr int kCsdVersionLen = 128;
extern wchar_t NT_CSD_VERSION[kCsdVersionLen];
extern USHORT NT_SP_MAJOR;
extern USHORT NT_SP_MINOR;
extern USHORT NT_SUITE;
extern UCHAR NT_TYPE;
// Non-Spoofable
extern ULONG REAL_NT_MAJOR;
extern ULONG REAL_NT_MINOR;
extern ULONG REAL_NT_BUILD;
// Our own generated strings
extern std::string NT_SERVICE_PACK;
extern std::string NT_FEATURE_VERSION;
extern std::string NT_POST_STRING;

// Internal string constants
inline constexpr wchar_t kOsInfoError[] = L"OSInfo Error";

namespace {
  constexpr unsigned long NTVER_40    = 0x0400UL;
  constexpr unsigned long NTVER_2K    = 0x0500UL;
  constexpr unsigned long NTVER_XP    = 0x0501UL;
  constexpr unsigned long NTVER_2K3   = 0x0502UL;
  constexpr unsigned long NTVER_VISTA = 0x0600UL;
  constexpr unsigned long NTVER_7     = 0x0601UL;
  constexpr unsigned long NTVER_8     = 0x0602UL;
  constexpr unsigned long NTVER_81    = 0x0603UL;
  constexpr unsigned long NTVER_10    = 0x0A00UL;
} // namespace

#ifndef OSINFO_STATIC_LIB
extern HINSTANCE gHinstDLL;

// Used to init .dll at attach, but use EnsureInitialized() for everything else.
static bool __cdecl InitOsInfoDll();

// Opposite of InitOsInfoDll, but only to be called privately.
static bool __cdecl DeInitOsInfoDLL();
#endif // !OSINFO_STATIC_LIB

// Ensures we are initialized.
static bool __cdecl EnsureInitialized();

// Pack hi/lo into DWORD hex
unsigned long __cdecl combineToHex(unsigned long high, unsigned long low);

// Uses undocumented function to get raw ntdll.dll version numbers.
static bool __cdecl GetRealVersions(ULONG* major, ULONG* minor, ULONG* build);

// Same as above, but uses GetRealVersions.
// Called by InitOsInfoDll/EnsureInitialized and as
// needed in other functions to grab the real, unspoofed version.
static bool __cdecl GetRealWinNTVersion();

// Main function that gets the Windows version number.
// Called by InitOsInfoDll/EnsureInitialized and as
// needed in other functions to grab the version.
static bool __cdecl GetWinNTVersion();

// Internal version parser
static std::string const __cdecl GetNTString();

// String Helper functions
static std::wstring const __cdecl StringToWstring(const std::string& str);
static std::string const __cdecl WstringToString(const std::wstring& wstr);

void __cdecl NotReachedImpl(const std::string& func_name);

#define __FUNC__ __func__

#ifndef NOTREACHED
 #define NOTREACHED()               \
   std::string func_name(__FUNC__); \
   NotReachedImpl(func_name);
#endif // NOTREACHED

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OSINFO_OS_INFO_H_
