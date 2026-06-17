#ifndef OSINFO_CPU_H_
#define OSINFO_CPU_H_

#include "framework.h"

#include "../public/cpuinfo.h"

// kernel32 is always mapped, so GetModuleHandleW is enough to reach it.
inline constexpr wchar_t kKernel32Dll[] = L"kernel32.dll";

// Vendor string leaf is 12 bytes on all x86 CPUs
inline constexpr size_t kVendorLeafSize = 12u;

// Human readable manufacturer string constants
inline constexpr wchar_t kIntelBrand[]   = L"Intel";
inline constexpr wchar_t kAmdBrand[]     = L"AMD";
inline constexpr wchar_t kCyrixBrand[]   = L"Cyrix";
inline constexpr wchar_t kViaBrand[]     = L"VIA";
inline constexpr wchar_t kCentaurBrand[] = L"Centaur";
inline constexpr wchar_t kUnknownBrand[] = L"Unknown";

#ifdef __cplusplus
extern "C" {
#endif

// Cached result, populated on the first successful GetCPUInfo() call.
extern CPUID_INFO g_CPUInfo;

extern bool g_cpu_detected;

DWORD __cdecl GetLogicalProcessorCount();

bool __cdecl DetectCpu(CPUID_INFO* info);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OSINFO_CPU_H_
