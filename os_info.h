#ifndef OSINFO_DLL_OS_INFO_H_
#define OSINFO_DLL_OS_INFO_H_

#include "os_info_dll.h"
#include <winver.h>

extern unsigned long WinVer;

extern unsigned long long WinVerFull;

extern HINSTANCE gHinstDLL;

namespace {
  static bool is_win11;
  static constexpr unsigned long NTVER_2K = 0x0500L;
  static constexpr unsigned long NTVER_XP = 0x0501L;
  static constexpr unsigned long NTVER_2K3 = 0x0502L;
  static constexpr unsigned long NTVER_VISTA = 0x0600L;
  static constexpr unsigned long NTVER_7 = 0x0601L;
  static constexpr unsigned long NTVER_8 = 0x0602L;
  static constexpr unsigned long NTVER_81 = 0x0603L;
  static constexpr unsigned long NTVER_10 = 0x0A00L;
  static constexpr unsigned long NTVER_11 = 0x0A00L;
}

static ULONG NT_MAJOR;

static ULONG NT_MINOR;

static ULONG NT_BUILD;

wchar_t NT_CSD_VERSION[128]{};

static USHORT NT_SP_MAJOR;

static USHORT NT_SP_MINOR;

static USHORT NT_SUITE;

static UCHAR NT_TYPE;

std::string NT_SERVICE_PACK;

std::string NT_FEATURE_VERSION;

std::string NT_POST_STRING;

float __cdecl concatToFloat(int major, int minor);

unsigned long __cdecl combineToHex(unsigned long high, unsigned long low);

static std::string const __cdecl GetNTString();

static std::wstring __cdecl StringToWstring(const std::string& str);

static std::string __cdecl WstringToString(const std::wstring& wstr);

inline void __cdecl NotReachedImpl(std::string func_name);

// Opposite of InitOsInfoDLL, but only to be called privately.
static const bool DeInitOsInfoDLL();

#define __FUNC__ __func__

#ifndef NOTREACHED
#define NOTREACHED() \
        std::string func_name(__FUNC__); \
        NotReachedImpl(func_name);
#endif // NOTREACHED

#endif // OSINFO_DLL_OS_INFO_H_
