#ifndef OSINFO_CPUID_H_
#define OSINFO_CPUID_H_

// NOTE: Don't include this file in your project, include libosinfo.h instead.

#include "lib_export.h"

#ifdef __cplusplus
extern "C" {
#endif

enum CPU_VENDOR {
  VENDOR_INTEL   = 0, // Also used for IBM
  VENDOR_AMD     = 1, // Also used for Geode and Hygon
  VENDOR_CYRIX   = 2, // Also used for National Semiconductor
  VENDOR_VIA     = 3, // Also used for Zhaoxin
  VENDOR_CENTAUR = 4, // Centaur
  VENDOR_UNKNOWN = 5  // Other
};

// Easy structure to pass around relevant CPUID info. This is a plain POD;
// callers should zero-initialize it (e.g. CPUID_INFO info = {};) and pass it to
// GetCPUInfo(), which fills every field.
struct CPUID_INFO {
  wchar_t raw_model[49];  // Model string, e.g. "Intel(R) Core(TM) i7-..." (48 chars + null)
  int vendor;             // The CPU brand, see CPU_VENDOR above.
  unsigned int num_cores; // Logical cores
  bool is_64_bit;         // Whether CPU is capable of long mode
  bool has_x87;           // On-chip x87 FPU. Available since i486DX/Pentium.
  bool has_mmx;           // MultiMedia Extensions
  bool has_3dnow;         // Originally AMD-only
  bool has_sse;           // Only single precision, Pentium III.
  bool has_sse2;          // Most modern compilers baseline target
  bool has_sse3;          // Chromium baseline, 98% of online systems support this.
  bool has_ssse3;         // Supplemental SSE3, not to be confused with regular SSE3
  bool has_sse41;         // Core 2 Duo "Wolfdale"
  bool has_sse42;         // End of x86 32 bit SIMD instructions
  bool has_xsave;         // Most AVX+ capable CPUs have this
  bool has_aes;           // aka AES-NI
  bool has_fma3;          // All Bulldozer and AVX2+ CPUs have this
  bool has_fma4;          // Only Bulldozer has this
  bool has_rdrand;        // Most CPUs since Ivy Bridge have this
  bool has_avx;           // Sandy Bridge/Bulldozer
  bool has_avx2;          // Haswell/Ryzen, commonly used for acceleration
  bool has_avx512f;       // AVX-512 "foundation" - All AVX-512 CPUs have this
  bool has_vmx;           // Virtual Machine Extensions
};

// Fills *cpuinfo with detected CPU information. Returns true on success, false
// if cpuinfo is null or the CPUID instruction is unavailable.
OSINFO_API bool __cdecl GetCPUInfo(CPUID_INFO* cpuinfo);

// Returns number of logical cores.
OSINFO_API unsigned int __cdecl NumCpuCores();

// Returns a string like "Intel" or "AMD".
OSINFO_API const wchar_t* __cdecl CpuVendor();

// Returns the full CPU model. Gets info directly from CPU registers, not the Windows registry.
OSINFO_API const wchar_t* __cdecl CpuModel();

// Returns whether the CPU if capable of x64. Note: Doesn't mean the app is executing x64 code.
OSINFO_API bool __cdecl IsCPU64BitCapable();

// If true, the OS can execute SSE - SSE4.2 instructions.
OSINFO_API bool __cdecl CanExecuteSSE();

// If true, the OS can execute AVX/AVX2 instructions.
OSINFO_API bool __cdecl CanExecuteAVX();

// If true, the OS can execute AVX-512 instructions.
OSINFO_API bool __cdecl CanExecuteAVX512();

// Gets the current average frequency of all CPU cores.
OSINFO_API unsigned long __cdecl CpuFreqAvg();

// Gets the max frequency the CPU is currently capable of.
OSINFO_API unsigned long __cdecl CpuFreqMax();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OSINFO_CPUID_H_
