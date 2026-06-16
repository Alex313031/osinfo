#ifndef OSINFO_CPU_H_
#define OSINFO_CPU_H_

#include "framework.h"

#include "../public/cpuinfo.h"

// kernel32 is always mapped, so GetModuleHandleW is enough to reach it.
inline constexpr wchar_t kKernel32Dll[] = L"kernel32.dll";

inline constexpr size_t kVendorLeafSize = 12u;

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
