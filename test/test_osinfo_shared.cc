// Copyright (c) 2026 Alex313031

// This tests all of the public OSInfo functions in the shared osinfo.dll, by
// dynamically resolving them with GetProcAddress using the typedefs in
// libosinfo.h. The DLL is NOT linked (see test/BUILD.gn data_deps), so every
// call goes through a runtime-resolved function pointer.

#include "test_framework.h"

#include <libosinfo.h>

// Print a bool as yes/no.
#define YN(b) ((b) ? L"yes" : L"no")

// Resolve an export into a local fn_<name> pointer of the given typedef type.
// (Named fn_<name> to avoid colliding with the dllimport declaration of <name>.)
#define RESOLVE(type, name)                                          \
  type fn_##name = reinterpret_cast<type>(GetProcAddress(lib, #name)); \
  if (fn_##name == nullptr) {                                        \
    std::cerr << "  !! missing export: " #name "\n";                 \
  }

namespace {
// Naive ASCII widen, see the static test for the rationale.
std::wstring Widen(const std::string& s) {
  return std::wstring(s.begin(), s.end());
}
} // namespace

int wmain(int argc, wchar_t* argv[]) {
  (void)argc;
  (void)argv;

  SetConsoleTitleW(APP_NAME);

  HMODULE lib = LoadLibraryW(L"osinfo.dll");
  if (lib == nullptr) {
    std::wcerr << L"Failed to load osinfo.dll (error " << GetLastError() << L")\n";
    system("pause");
    return 1;
  }

  std::wcout << L"===== OSInfo Shared DLL Test (GetProcAddress) =====\n\n";

  // --- Resolve every export through its libosinfo.h typedef ---
  RESOLVE(pDllGetVersion, DllGetVersion);
  RESOLVE(pGetOSNameW, GetOSNameW);
  RESOLVE(pGetOSNameA, GetOSNameA);
  RESOLVE(pGetWinVersionW, GetWinVersionW);
  RESOLVE(pGetWinVersionA, GetWinVersionA);
  RESOLVE(pGetOsInfoVersionW, GetOsInfoVersionW);
  RESOLVE(pGetOsInfoVersionA, GetOsInfoVersionA);
  RESOLVE(pGetServicePackNumber, GetServicePackNumber);
  RESOLVE(pGetServicePackW, GetServicePackW);
  RESOLVE(pGetServicePackA, GetServicePackA);
  RESOLVE(pGetReportedNTVer, GetReportedNTVer);
  RESOLVE(pGetRealNTVer, GetRealNTVer);
  RESOLVE(pGetReportedShortNTVer, GetReportedShortNTVer);
  RESOLVE(pGetRealShortNTVer, GetRealShortNTVer);
  RESOLVE(pIsWoW64, IsWoW64);
  RESOLVE(pIsWinNT4, IsWinNT4);
  RESOLVE(pIsWin2K, IsWin2K);
  RESOLVE(pIsWinXP, IsWinXP);
  RESOLVE(pIsWin2003, IsWin2003);
  RESOLVE(pIsWinVista, IsWinVista);
  RESOLVE(pIsWin7, IsWin7);
  RESOLVE(pIsWin8, IsWin8);
  RESOLVE(pIsWin8_1, IsWin8_1);
  RESOLVE(pIsWin10, IsWin10);
  RESOLVE(pIsWin11, IsWin11);
  RESOLVE(pIsWin, IsWin);
  RESOLVE(pIsAtLeast, IsAtLeast);
  RESOLVE(pIsAtMost, IsAtMost);
  RESOLVE(pIsWinNewerThan, IsWinNewerThan);
  RESOLVE(pIsWinOlderThan, IsWinOlderThan);
  RESOLVE(pGetCPUInfo, GetCPUInfo);
  RESOLVE(pNumCpuCores, NumCpuCores);
  RESOLVE(pCpuVendor, CpuVendor);
  RESOLVE(pCpuModel, CpuModel);
  RESOLVE(pIsCPU64BitCapable, IsCPU64BitCapable);
  RESOLVE(pCanExecuteSSE, CanExecuteSSE);
  RESOLVE(pCanExecuteAVX, CanExecuteAVX);
  RESOLVE(pCanExecuteAVX512, CanExecuteAVX512);
  RESOLVE(pCpuFreqAvg, CpuFreqAvg);
  RESOLVE(pCpuFreqMax, CpuFreqMax);

  // --- Call each one (guarded in case an export was missing) ---
  std::wcout << L"-- OS --\n";
  if (fn_GetOSNameW)        std::wcout << L"GetOSNameW:            " << fn_GetOSNameW() << L"\n";
  if (fn_GetOSNameA)        std::wcout << L"GetOSNameA:            " << Widen(fn_GetOSNameA()) << L"\n";
  if (fn_GetWinVersionW)    std::wcout << L"GetWinVersionW:        " << fn_GetWinVersionW() << L"\n";
  if (fn_GetWinVersionA)    std::wcout << L"GetWinVersionA:        " << Widen(fn_GetWinVersionA()) << L"\n";
  if (fn_GetOsInfoVersionW) std::wcout << L"GetOsInfoVersionW:     " << fn_GetOsInfoVersionW() << L"\n";
  if (fn_GetOsInfoVersionA) std::wcout << L"GetOsInfoVersionA:     " << Widen(fn_GetOsInfoVersionA()) << L"\n";

  if (fn_DllGetVersion) {
    DLLVERSIONINFO dvi = {};
    dvi.cbSize = sizeof(dvi);
    if (fn_DllGetVersion(&dvi) == S_OK) {
      std::wcout << L"DllGetVersion:         " << dvi.dwMajorVersion << L"." << dvi.dwMinorVersion
                 << L"." << dvi.dwBuildNumber << L"\n";
    }
  }

  if (fn_GetServicePackNumber) std::wcout << L"GetServicePackNumber:  " << fn_GetServicePackNumber() << L"\n";
  if (fn_GetServicePackW)   std::wcout << L"GetServicePackW:       " << fn_GetServicePackW() << L"\n";
  if (fn_GetServicePackA)   std::wcout << L"GetServicePackA:       " << Widen(fn_GetServicePackA()) << L"\n";

  std::wcout << std::hex << std::showbase;
  if (fn_GetReportedNTVer)      std::wcout << L"GetReportedNTVer:      " << fn_GetReportedNTVer() << L"\n";
  if (fn_GetRealNTVer)          std::wcout << L"GetRealNTVer:          " << fn_GetRealNTVer() << L"\n";
  if (fn_GetReportedShortNTVer) std::wcout << L"GetReportedShortNTVer: " << fn_GetReportedShortNTVer() << L"\n";
  if (fn_GetRealShortNTVer)     std::wcout << L"GetRealShortNTVer:     " << fn_GetRealShortNTVer() << L"\n";
  std::wcout << std::dec << std::noshowbase;

  std::wcout << L"\n-- Version checks --\n";
  if (fn_IsWoW64)     std::wcout << L"IsWoW64:               " << YN(fn_IsWoW64()) << L"\n";
  if (fn_IsWinNT4)    std::wcout << L"IsWinNT4:              " << YN(fn_IsWinNT4()) << L"\n";
  if (fn_IsWin2K)     std::wcout << L"IsWin2K:               " << YN(fn_IsWin2K()) << L"\n";
  if (fn_IsWinXP)     std::wcout << L"IsWinXP:               " << YN(fn_IsWinXP()) << L"\n";
  if (fn_IsWin2003)   std::wcout << L"IsWin2003:             " << YN(fn_IsWin2003()) << L"\n";
  if (fn_IsWinVista)  std::wcout << L"IsWinVista:            " << YN(fn_IsWinVista()) << L"\n";
  if (fn_IsWin7)      std::wcout << L"IsWin7:                " << YN(fn_IsWin7()) << L"\n";
  if (fn_IsWin8)      std::wcout << L"IsWin8:                " << YN(fn_IsWin8()) << L"\n";
  if (fn_IsWin8_1)    std::wcout << L"IsWin8_1:              " << YN(fn_IsWin8_1()) << L"\n";
  if (fn_IsWin10)     std::wcout << L"IsWin10:               " << YN(fn_IsWin10()) << L"\n";
  if (fn_IsWin11)     std::wcout << L"IsWin11:               " << YN(fn_IsWin11()) << L"\n";
  if (fn_IsWin)       std::wcout << L"IsWin(kWin10):         " << YN(fn_IsWin(kWin10)) << L"\n";
  if (fn_IsAtLeast)   std::wcout << L"IsAtLeast(kWin7):      " << YN(fn_IsAtLeast(kWin7)) << L"\n";
  if (fn_IsAtMost)    std::wcout << L"IsAtMost(kWinVista):   " << YN(fn_IsAtMost(kWinVista)) << L"\n";
  if (fn_IsWinNewerThan) std::wcout << L"IsWinNewerThan(kWinXP):" << YN(fn_IsWinNewerThan(kWinXP)) << L"\n";
  if (fn_IsWinOlderThan) std::wcout << L"IsWinOlderThan(kWin11):" << YN(fn_IsWinOlderThan(kWin11)) << L"\n";

  std::wcout << L"\n-- CPU --\n";
  if (fn_CpuVendor)         std::wcout << L"CpuVendor:             " << fn_CpuVendor() << L"\n";
  if (fn_CpuModel)          std::wcout << L"CpuModel:              " << fn_CpuModel() << L"\n";
  if (fn_NumCpuCores)       std::wcout << L"NumCpuCores:           " << fn_NumCpuCores() << L"\n";
  if (fn_IsCPU64BitCapable) std::wcout << L"IsCPU64BitCapable:     " << YN(fn_IsCPU64BitCapable()) << L"\n";
  if (fn_CanExecuteSSE)     std::wcout << L"CanExecuteSSE:         " << YN(fn_CanExecuteSSE()) << L"\n";
  if (fn_CanExecuteAVX)     std::wcout << L"CanExecuteAVX:         " << YN(fn_CanExecuteAVX()) << L"\n";
  if (fn_CanExecuteAVX512)  std::wcout << L"CanExecuteAVX512:      " << YN(fn_CanExecuteAVX512()) << L"\n";
  if (fn_CpuFreqAvg)        std::wcout << L"CpuFreqAvg:            " << fn_CpuFreqAvg() << L" MHz.\n";
  if (fn_CpuFreqMax)        std::wcout << L"CpuFreqMax:            " << fn_CpuFreqMax() << L" MHz.\n";

  std::wcout << L"\n-- CPUID_INFO --\n";
  if (fn_GetCPUInfo) {
    CPUID_INFO ci = {};
    if (fn_GetCPUInfo(&ci)) {
      std::wcout << L"GetCPUInfo.raw_model:  " << ci.raw_model << L"\n";
      std::wcout << L"GetCPUInfo.num_cores:  " << ci.num_cores << L"\n";
    }
  }

  FreeLibrary(lib);
  std::wcout << std::endl;
  system("pause");
  return 0;
}
