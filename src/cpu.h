#ifndef OSINFO_CPU_H_
#define OSINFO_CPU_H_

#include "../public/cpuinfo.h"
#include "framework.h"

// kernel32 is always mapped, so GetModuleHandleW is enough to reach it.
inline constexpr wchar_t kKernel32Dll[] = L"kernel32.dll";
inline constexpr wchar_t kNtDllDll[]    = L"ntdll.dll";    // Same for ntdll.dll
inline constexpr wchar_t kPowrProfDll[] = L"powrprof.dll"; // Loaded on demand (not always mapped)

inline constexpr size_t kModelStrSize = 48u;

// Human readable manufacturer string constants
inline constexpr wchar_t kIntelBrand[]   = L"Intel";
inline constexpr wchar_t kAmdBrand[]     = L"AMD";
inline constexpr wchar_t kCyrixBrand[]   = L"Cyrix";
inline constexpr wchar_t kViaBrand[]     = L"VIA";
inline constexpr wchar_t kCentaurBrand[] = L"Centaur";
inline constexpr wchar_t kUnknownBrand[] = L"Unknown";

// Not included in powerbase.h
#ifndef _PROCESSOR_POWER_INFORMATION
typedef struct _PROCESSOR_POWER_INFORMATION {
  ULONG Number;
  ULONG MaxMhz;
  ULONG CurrentMhz;
  ULONG MhzLimit;
  ULONG MaxIdleState;
  ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
#endif // !_PROCESSOR_POWER_INFORMATION

/* Miscellaneous constants */
#define CPUID_EAX          0          // Index of EAX value in array.
#define CPUID_EBX          1          // Index of EBX value in array.
#define CPUID_ECX          2          // Index of ECX value in array
#define CPUID_EDX          3          // Index of EDX value in array.
#define CPUID_STD_BASE     0x00000000 // Standard leaves base.
#define CPUID_EXT_BASE     0x80000000 // Extended leaves base.
#define CPUID_BRAND_1ST    0x80000002 // First brand string leaf.

#ifdef __cplusplus
extern "C" {
#endif

DWORD __cdecl GetLogicalProcessorCount();

bool __cdecl DetectCpu(CPUID_INFO* info);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OSINFO_CPU_H_
