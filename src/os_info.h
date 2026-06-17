#ifndef OSINFO_OS_INFO_H_
#define OSINFO_OS_INFO_H_

#include "framework.h"

// Manual relative include
#include "../public/libosinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL(WINAPI* IS_WOW64_PROCESS_)(HANDLE hProcess, PBOOL Wow64Process);

extern unsigned long WinVer;

extern unsigned long long WinVerFull;

inline constexpr wchar_t kOsInfoError[] = L"OSInfo Error";

#ifndef OSINFO_STATIC_LIB
 extern HINSTANCE gHinstDLL;
#endif // !OSINFO_STATIC_LIB

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

namespace {
  constexpr unsigned long NTVER_40    = 0x0400L;
  constexpr unsigned long NTVER_2K    = 0x0500L;
  constexpr unsigned long NTVER_XP    = 0x0501L;
  constexpr unsigned long NTVER_2K3   = 0x0502L;
  constexpr unsigned long NTVER_VISTA = 0x0600L;
  constexpr unsigned long NTVER_7     = 0x0601L;
  constexpr unsigned long NTVER_8     = 0x0602L;
  constexpr unsigned long NTVER_81    = 0x0603L;
  constexpr unsigned long NTVER_10    = 0x0A00L;
  constexpr unsigned long NTVER_11    = 0x0A00L;
} // namespace

static ULONG NT_MAJOR;

static ULONG NT_MINOR;

static ULONG NT_BUILD;

static wchar_t NT_CSD_VERSION[128]{};

static USHORT NT_SP_MAJOR;

static USHORT NT_SP_MINOR;

static USHORT NT_SUITE;

static UCHAR NT_TYPE;

static std::string NT_SERVICE_PACK;

static std::string NT_FEATURE_VERSION;

static std::string NT_POST_STRING;

unsigned long __cdecl combineToHex(unsigned long high, unsigned long low);

static std::string const __cdecl GetNTString();

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
