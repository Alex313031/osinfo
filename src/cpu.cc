#include "cpu.h"

// Functions to retrieve CPU info: Num. Cores, Model, and SIMD instruction levels support.

// CPUID is accessed through compiler intrinsics so the same code builds on 32-
// and 64-bit MSVC and GCC/MinGW (MSVC forbids inline asm on x64, and the GCC
// <cpuid.h> macros correctly preserve EBX under 32-bit PIC):
//   MSVC            -> __cpuid / __cpuidex            (<intrin.h>)
//   GCC/Clang/MinGW -> __cpuid_count / __get_cpuid_max (<cpuid.h>)
#if defined(_MSC_VER)
 #include <intrin.h>
#else
 #include <cpuid.h>
#endif

#include <powrprof.h>

CPUID_INFO g_CPUInfo{};

// True once g_CPUInfo has been populated (success or not), so we only probe once.
bool g_cpu_detected = false;

struct VendorMap {
  const char* id;
  int vendor;
};

// Returns bit n of v as a bool.
static inline bool __cdecl BitCheck(uint32_t v, unsigned int n) {
  return ((v >> n) & 1u) != 0u;
}

// Fills regs[4] = {EAX, EBX, ECX, EDX} for the given CPUID leaf/subleaf.
static void __cdecl FillRegInfo(uint32_t leaf, uint32_t subleaf, uint32_t regs[4]) {
#if defined(_MSC_VER)
  int r[4];
  __cpuidex(r, static_cast<int>(leaf), static_cast<int>(subleaf));
  regs[CPUID_EAX] = static_cast<uint32_t>(r[CPUID_EAX]);
  regs[CPUID_EBX] = static_cast<uint32_t>(r[CPUID_EBX]);
  regs[CPUID_ECX] = static_cast<uint32_t>(r[CPUID_ECX]);
  regs[CPUID_EDX] = static_cast<uint32_t>(r[CPUID_EDX]);
#else
  unsigned int a = 0u, b = 0u, c = 0u, d = 0u;
  __cpuid_count(leaf, subleaf, a, b, c, d);
  regs[CPUID_EAX] = a;
  regs[CPUID_EBX] = b;
  regs[CPUID_ECX] = c;
  regs[CPUID_EDX] = d;
#endif
}

// Highest supported leaf for a CPUID range base (0 for standard, 0x80000000 for
// extended). Returns 0 if the range / CPUID itself is unavailable.
static uint32_t __cdecl GetLeafCount(uint32_t base) {
#if defined(_MSC_VER)
  uint32_t regs[4];
  FillRegInfo(base, 0u, regs);
  // EAX < base means the range is not implemented.
  return regs[CPUID_EAX] >= base ? regs[CPUID_EAX] : 0u;
#else
  // __get_cpuid_max also performs the CPUID-presence check on 32-bit and
  // returns 0 when CPUID is unsupported.
  return __get_cpuid_max(base, nullptr);
#endif
}

// XCR0 bits the OS must have enabled for AVX state to survive context switches:
// bit 1 (SSE/XMM state) and bit 2 (AVX/YMM state).
static constexpr uint64_t kXcr0AvxMask = 0x6u;

// AVX-512 additionally needs bit 5 (opmask k0-k7), bit 6 (upper halves of
// ZMM0-15) and bit 7 (ZMM16-31), on top of the AVX (XMM+YMM) bits.
static constexpr uint64_t kXcr0Avx512Mask = 0xE6u;

// Reads extended control register XCR0 via XGETBV. ONLY call this when CPUID
// reports OSXSAVE (CPUID.1:ECX[27]); executing XGETBV otherwise faults (#UD).
static uint64_t __cdecl ReadXcr0() {
#if defined(_MSC_VER)
  return _xgetbv(0);
#else
  uint32_t eax = 0u, edx = 0u;
  // XGETBV with ECX=0. Emitted as raw bytes so it assembles without -mxsave.
  __asm__ volatile(".byte 0x0f, 0x01, 0xd0" : "=a"(eax), "=d"(edx) : "c"(0u));
  return (static_cast<uint64_t>(edx) << 32) | eax;
#endif
}

// Maps the 12-byte CPUID leaf-0 vendor string to a CPU_VENDOR.
static int __cdecl MapVendor(const char vendor[kVendorLeafSize]) {
  // Anything not listed here falls through to VENDOR_UNKNOWN.
  static const VendorMap kVendors[] = {
      {"GenuineIntel", VENDOR_INTEL},   {"AuthenticAMD", VENDOR_AMD}, {"HygonGenuine", VENDOR_AMD},
      {"CyrixInstead", VENDOR_CYRIX},   {"VIA VIA VIA ", VENDOR_VIA}, {"  Shanghai  ", VENDOR_VIA},
      {"CentaurHauls", VENDOR_CENTAUR},
  };
  for (const VendorMap& v : kVendors) {
    if (memcmp(vendor, v.id, kVendorLeafSize) == 0) {
      return v.vendor;
    }
  }
  return VENDOR_UNKNOWN;
}

// Copies the (ASCII) CPU brand string into the wide raw_model field, trimming
// the leading padding spaces that brand strings commonly carry.
static void __cdecl FormatBrandString(CPUID_INFO* info, const char brand[48]) {
  // Bounded, null-terminated source copy first.
  char ascii[49];
  memcpy(ascii, brand, 48);
  ascii[48] = '\0';

  const char* start = ascii;
  while (*start == ' ') {
    ++start;
  }

  size_t i = 0u;
  for (; start[i] != '\0' && i < 48; ++i) {
    info->raw_model[i] = static_cast<wchar_t>(static_cast<unsigned char>(start[i]));
  }
  info->raw_model[i] = L'\0';
}

// Returns the OS-reported logical processor count (native count via
// GetNativeSystemInfo on XP+, GetSystemInfo otherwise); always at least 1.
DWORD __cdecl GetLogicalProcessorCount() {
  SYSTEM_INFO si = {};

  // GetNativeSystemInfo is XP+ only. Resolve it dynamically and use
  // older GetSystemInfo on NT 4.0/2000.
  typedef void(WINAPI * FnGetNativeSystemInfo)(LPSYSTEM_INFO);
  static FnGetNativeSystemInfo pfnGetNativeSystemInfo = nullptr;
  static bool s_resolved                              = false;
  if (!s_resolved) {
    HMODULE hKernel32      = GetModuleHandleW(kKernel32Dll);
    pfnGetNativeSystemInfo = reinterpret_cast<FnGetNativeSystemInfo>(
        hKernel32 ? GetProcAddress(hKernel32, "GetNativeSystemInfo") : nullptr);
    s_resolved = true;
  }

  if (pfnGetNativeSystemInfo) {
    pfnGetNativeSystemInfo(&si);
  } else {
    GetSystemInfo(&si);
  }
  return (si.dwNumberOfProcessors > 0) ? si.dwNumberOfProcessors : 1;
}

// Probes the CPU and fills *info. Returns false if CPUID is unavailable (in
// which case only OS-derived fields like num_cores are valid).
bool __cdecl DetectCpu(CPUID_INFO* info) {
  *info = CPUID_INFO{};

  // Logical core count comes from the OS, not CPUID: the CPUID topology leaves
  // are vendor-specific. Note this is the count for the current processor group.
  info->num_cores = static_cast<unsigned int>(GetLogicalProcessorCount());

  const uint32_t max_basic = GetLeafCount(CPUID_STD_BASE);
  if (max_basic == 0u) {
    // No CPUID (pre-Pentium) - nothing more we can read.
    info->vendor = VENDOR_UNKNOWN;
    lstrcpynW(info->raw_model, kUnknownBrand, 49);
    return false;
  }

  uint32_t regs[4];

  // Leaf 0: vendor string (EBX, EDX, ECX).
  FillRegInfo(CPUID_STD_BASE, 0u, regs);
  char vendor[kVendorLeafSize];
  memcpy(vendor + 0, &regs[CPUID_EBX], 4);
  memcpy(vendor + 4, &regs[CPUID_EDX], 4);
  memcpy(vendor + 8, &regs[CPUID_ECX], 4);
  info->vendor = MapVendor(vendor);

  // Leaf 1: standard feature flags.
  if (max_basic >= 1u) {
    FillRegInfo(1u, 0u, regs);
    const uint32_t edx = regs[CPUID_EDX];
    const uint32_t ecx = regs[CPUID_ECX];
    info->has_x87      = BitCheck(edx, 0u);
    info->has_mmx      = BitCheck(edx, 23u);
    info->has_sse      = BitCheck(edx, 25u);
    info->has_sse2     = BitCheck(edx, 26u);
    info->has_sse3     = BitCheck(ecx, 0u);
    info->has_vmx      = BitCheck(ecx, 5u);
    info->has_ssse3    = BitCheck(ecx, 9u);
    info->has_fma3     = BitCheck(ecx, 12u);
    info->has_sse41    = BitCheck(ecx, 19u);
    info->has_sse42    = BitCheck(ecx, 20u);
    info->has_aes      = BitCheck(ecx, 25u);
    info->has_xsave    = BitCheck(ecx, 26u);
    info->has_avx      = BitCheck(ecx, 28u);
    info->has_rdrand   = BitCheck(ecx, 30u);
  }

  // Leaf 7, subleaf 0: extended features.
  if (max_basic >= 7u) {
    FillRegInfo(7u, 0u, regs);
    const uint32_t ebx = regs[CPUID_EBX];
    info->has_avx2     = BitCheck(ebx, 5u);
    info->has_avx512f  = BitCheck(ebx, 16u);
  }

  // Extended leaves.
  const uint32_t max_ext = GetLeafCount(CPUID_EXT_BASE);
  if (max_ext >= 0x80000001) {
    FillRegInfo(0x80000001, 0u, regs);
    info->has_fma4  = BitCheck(regs[CPUID_ECX], 16u);
    info->has_3dnow = BitCheck(regs[CPUID_EDX], 31u);
    info->is_64_bit = BitCheck(regs[CPUID_EDX], 29u); // Long Mode (LM) bit.
  }

  // Brand string lives in extended leaves 0x80000002..0x80000004 (48 bytes).
  if (max_ext >= 0x80000004) {
    char brand[48];
    for (uint32_t i = 0u; i < 3u; ++i) {
      FillRegInfo(CPUID_BRAND_1ST + i, 0u, regs);
      memcpy(brand + i * 16, regs, 16);
    }
    FormatBrandString(info, brand);
  } else {
    lstrcpynW(info->raw_model, kUnknownBrand, 49);
  }

  return true;
}

OSINFO_API bool __cdecl GetCPUInfo(CPUID_INFO* cpuinfo) {
  if (cpuinfo == nullptr) {
    return false;
  }
  if (!g_cpu_detected) {
    DetectCpu(&g_CPUInfo);
    g_cpu_detected = true;
  }
  *cpuinfo = g_CPUInfo;
  return true;
}

OSINFO_API unsigned int __cdecl NumCpuCores() {
  return static_cast<unsigned int>(GetLogicalProcessorCount());
}

OSINFO_API const wchar_t* __cdecl CpuVendor() {
  CPUID_INFO vendor_info{};
  if (!GetCPUInfo(&vendor_info)) {
    return L"Failed to get CPU Vendor.";
  }
  const int vendor_id = vendor_info.vendor;
  switch (vendor_id) {
    case VENDOR_INTEL:
      return kIntelBrand;
    case VENDOR_AMD:
      return kAmdBrand;
    case VENDOR_CYRIX:
      return kCyrixBrand;
    case VENDOR_VIA:
      return kViaBrand;
    case VENDOR_CENTAUR:
      return kCentaurBrand;
    case VENDOR_UNKNOWN:
      return kUnknownBrand;
    default:
      return kUnknownBrand;
  }
}

OSINFO_API const wchar_t* __cdecl CpuModel() {
  if (!GetCPUInfo(&g_CPUInfo)) {
    return L"Failed to get CPU Model.";
  }
  // raw_model lives in the static g_CPUInfo cache, so the pointer stays valid.
  return g_CPUInfo.raw_model;
}

OSINFO_API bool __cdecl IsCPU64BitCapable() {
  CPUID_INFO lm_info{};
  if (!GetCPUInfo(&lm_info)) {
    return false;
  }
  return lm_info.is_64_bit;
}

OSINFO_API bool __cdecl CanExecuteSSE() {
  // SSE OS-enablement (CR4.OSFXSR) is not exposed via CPUID, so ask Windows.
  // IsProcessorFeaturePresent reports CPU and OS support together. Resolve it
  // dynamically: it is absent on NT 4.0, where SSE state is never OS-saved
  // anyway, so a missing export correctly maps to "false".
  typedef BOOL(WINAPI * FnIsProcessorFeaturePresent)(DWORD);
  static FnIsProcessorFeaturePresent pfnIsProcessorFeaturePresent = nullptr;
  static bool s_resolved                                          = false;
  if (!s_resolved) {
    HMODULE hKernel32            = GetModuleHandleW(kKernel32Dll);
    pfnIsProcessorFeaturePresent = reinterpret_cast<FnIsProcessorFeaturePresent>(
        hKernel32 ? GetProcAddress(hKernel32, "IsProcessorFeaturePresent") : nullptr);
    s_resolved = true;
  }
  if (!pfnIsProcessorFeaturePresent) {
    return false;
  }
  return pfnIsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE) != FALSE;
}

OSINFO_API bool __cdecl CanExecuteAVX() {
  // The CPU must support CPUID leaf 1 to report AVX/OSXSAVE.
  if (GetLeafCount(CPUID_STD_BASE) < 1u) {
    return false;
  }
  uint32_t regs[4];
  FillRegInfo(1u, 0u, regs);
  const uint32_t ecx = regs[CPUID_ECX];
  // OSXSAVE (bit 27) means the OS enabled XSAVE; AVX (bit 28) is CPU support.
  // Both are required before XGETBV is even safe to execute.
  if (!BitCheck(ecx, 27u) || !BitCheck(ecx, 28u)) {
    return false;
  }
  // Finally confirm the OS is actually preserving XMM+YMM state.
  return (ReadXcr0() & kXcr0AvxMask) == kXcr0AvxMask;
}

OSINFO_API bool __cdecl CanExecuteAVX512() {
  // OSXSAVE is reported in leaf 1; the AVX512F CPU bit lives in leaf 7.
  if (GetLeafCount(CPUID_STD_BASE) < 7u) {
    return false;
  }
  uint32_t regs[4];
  FillRegInfo(1u, 0u, regs);
  if (!BitCheck(regs[CPUID_ECX], 27u)) { // OSXSAVE: OS enabled XSAVE.
    return false;
  }
  FillRegInfo(7u, 0u, regs);
  if (!BitCheck(regs[CPUID_EBX], 16u)) { // AVX512F: CPU support.
    return false;
  }
  // OS must preserve opmask + ZMM state in addition to XMM/YMM.
  return (ReadXcr0() & kXcr0Avx512Mask) == kXcr0Avx512Mask;
}

// Signature of powrprof!CallNtPowerInformation (Windows 2000+).
typedef NTSTATUS(
    WINAPI* FnCallNtPowerInformation)(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);

// Fills `out` with one PROCESSOR_POWER_INFORMATION per logical core. The
// CallNtPowerInformation export is resolved dynamically rather than statically
// imported, so osinfo.dll still loads on NT 4.0 (where powrprof.dll lacks it);
// there this simply returns false and callers report 0.
static bool __cdecl QueryProcessorPower(std::vector<PROCESSOR_POWER_INFORMATION>& out) {
  using FnCallNtPowerInformation =
      NTSTATUS(WINAPI*)(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);
  static FnCallNtPowerInformation pfnCallNtPowerInformation = nullptr;
  static bool s_resolved                                    = false;
  if (!s_resolved) {
    // powrprof.dll is not always mapped, so LoadLibrary (not GetModuleHandle).
    // The handle is intentionally left loaded for the cached pointer's lifetime.
    HMODULE hPowrProf         = LoadLibraryW(kPowrProfDll);
    pfnCallNtPowerInformation = reinterpret_cast<FnCallNtPowerInformation>(
        hPowrProf ? GetProcAddress(hPowrProf, "CallNtPowerInformation") : nullptr);
    s_resolved = true;
  }
  if (!pfnCallNtPowerInformation) {
    return false;
  }

  out.resize(NumCpuCores());
  const NTSTATUS status = pfnCallNtPowerInformation(
      ProcessorInformation, nullptr, 0, &out[0],
      static_cast<ULONG>(sizeof(PROCESSOR_POWER_INFORMATION) * out.size()));
  return status == STATUS_SUCCESS;
}

OSINFO_API unsigned long __cdecl CpuFreqAvg() {
  std::vector<PROCESSOR_POWER_INFORMATION> powerInfo;
  if (!QueryProcessorPower(powerInfo)) {
    return 0UL;
  }
  ULONG total = 0UL;
  for (const PROCESSOR_POWER_INFORMATION& pi : powerInfo) {
    total += pi.CurrentMhz;
  }
  return total / static_cast<ULONG>(powerInfo.size());
}

OSINFO_API unsigned long __cdecl CpuFreqMax() {
  std::vector<PROCESSOR_POWER_INFORMATION> powerInfo;
  if (!QueryProcessorPower(powerInfo)) {
    return 0UL;
  }
  return powerInfo[0].MaxMhz;
}
