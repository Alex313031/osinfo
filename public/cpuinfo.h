#ifndef OSINFO_CPUID_H_
#define OSINFO_CPUID_H_

// NOTE: Don't include this file in your project, include libosinfo.h instead.

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

/* Miscellaneous constants */
#define CPUID_EAX           0          // Index of EAX value in array.
#define CPUID_EBX           1          // Index of EBX value in array.
#define CPUID_ECX           2          // Index of ECX value in array
#define CPUID_EDX           3          // Index of EDX value in array.
#define CPUID_STD_BASE      0x00000000 // Standard leaves base.
#define CPUID_EXT_BASE      0x80000000 // Extended leaves base.
#define CPUID_BRAND_1ST     0x80000002 // First brand string leaf.
#define CPUID_CENTAUR_BASE  0xC0000000 // Centaur leaves base.

#ifdef __cplusplus
extern "C" {
#endif

enum CPU_VENDOR {
  VENDOR_INTEL = 0, // Also used for IBM
  VENDOR_AMD = 1, // Also used for Geode and Hygon
  VENDOR_CYRIX = 2, // Also used for National Semiconductor
  VENDOR_VIA = 3, // Also used for Zhaoxin
  VENDOR_CENTAUR = 4, // Centaur
  VENDOR_UNKNOWN = 5
};

// Easy structure to pass around relevant CPUID info. This is a plain POD;
// callers should zero-initialize it (e.g. CPUID_INFO info = {};) and pass it to
// GetCPUInfo(), which fills every field.
struct CPUID_INFO {
  wchar_t raw_model[49];  // Brand string, e.g. "Intel(R) Core(TM) i7-..." (48 chars + null)
  int vendor;             // See CPU_VENDOR
  unsigned int num_cores; // Logical cores
  bool is_64_bit;   // Whether CPU is capable of long mode
  bool has_x87;     // On-chip x87 FPU. Available since i486DX/Pentium.
  bool has_mmx;     // MultiMedia Extensions
  bool has_3dnow;   // Originally AMD-only
  bool has_sse;     // Only single precision, Pentium III.
  bool has_sse2;    // Most modern compilers baseline target
  bool has_sse3;    // Chromium baseline, 98% of online systems support this.
  bool has_ssse3;   // Supplemental SSE3, not to be confused with regular SSE3
  bool has_sse41;   // Core 2 Duo "Wolfdale"
  bool has_sse42;   // End of x86 32 bit SIMD instructions
  bool has_xsave;   // Most AVX+ capable CPUs have this
  bool has_aes;     // aka AES-NI
  bool has_fma3;    // All Bulldozer and AVX2+ CPUs have this
  bool has_fma4;    // Only Bulldozer has this
  bool has_rdrand;  // Most CPUs since Ivy Bridge have this
  bool has_avx;     // Sandy Bridge/Bulldozer
  bool has_avx2;    // Haswell/Ryzen, commonly used for acceleration
  bool has_avx512f; // AVX-512 "foundation" - All AVX-512 CPUs have this
  bool has_vmx;     // Virtual Machine Extensions
};

// Fills *cpuinfo with detected CPU information. Returns true on success, false
// if cpuinfo is null or the CPUID instruction is unavailable.
OSINFO_API bool __cdecl GetCPUInfo(CPUID_INFO *cpuinfo);

OSINFO_API unsigned int __cdecl NumCpuCores();

OSINFO_API const wchar_t* __cdecl CpuVendor();

OSINFO_API const wchar_t* __cdecl CpuModel();

OSINFO_API bool __cdecl IsCPU64BitCapable();

OSINFO_API bool __cdecl CanExecuteSSE();

OSINFO_API bool __cdecl CanExecuteAVX();

OSINFO_API bool __cdecl CanExecuteAVX512();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OSINFO_CPUID_H_
