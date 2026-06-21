// Copyright (c) 2026 Alex313031

// This tests all of the public OSInfo functions in the static lib,
// directly calling functions declared in libosinfo.h.

#include "test_framework.h"

#include <libosinfo.h>

// Print a bool as yes/no.
#define YN(value) ((value) ? L"yes" : L"no")

namespace {
// Naive ASCII widen, just so the narrow-string getters can print next to the
// wide ones. OS strings are ASCII, so this is fine for a console test.
std::wstring Widen(const std::string& s) {
  return std::wstring(s.begin(), s.end());
}
} // namespace

int wmain(int argc, wchar_t* argv[]) {
  (void)argc;
  (void)argv;

  SetConsoleTitleW(APP_NAME);

  std::wcout << L"===== OSInfo Static Lib Test (direct calls) =====\n\n";

  std::wcout << L"-- OS --\n";
  std::wcout << L"GetOSNameW:            " << GetOSNameW() << L"\n";
  std::wcout << L"GetOSNameA:            " << Widen(GetOSNameA()) << L"\n";
  std::wcout << L"GetWinVersionW:        " << GetWinVersionW() << L"\n";
  std::wcout << L"GetWinVersionA:        " << Widen(GetWinVersionA()) << L"\n";
  std::wcout << L"GetOsInfoVersionW:     " << GetOsInfoVersionW() << L"\n";
  std::wcout << L"GetOsInfoVersionA:     " << Widen(GetOsInfoVersionA()) << L"\n";
  std::wcout << L"GetServicePackNumber:  " << GetServicePackNumber() << L"\n";
  std::wcout << L"GetServicePackW:       " << GetServicePackW() << L"\n";
  std::wcout << L"GetServicePackA:       " << Widen(GetServicePackA()) << L"\n";

  std::wcout << std::hex << std::showbase;
  std::wcout << L"GetReportedNTVer:      " << GetReportedNTVer() << L"\n";
  std::wcout << L"GetRealNTVer:          " << GetRealNTVer() << L"\n";
  std::wcout << L"GetReportedShortNTVer: " << GetReportedShortNTVer() << L"\n";
  std::wcout << L"GetRealShortNTVer:     " << GetRealShortNTVer() << L"\n";
  std::wcout << std::dec << std::noshowbase;

  std::wcout << L"\n-- Version checks --\n";
  std::wcout << L"IsWoW64:               " << YN(IsWoW64()) << L"\n";
  std::wcout << L"IsWinNT4:              " << YN(IsWinNT4()) << L"\n";
  std::wcout << L"IsWin2K:               " << YN(IsWin2K()) << L"\n";
  std::wcout << L"IsWinXP:               " << YN(IsWinXP()) << L"\n";
  std::wcout << L"IsWin2003:             " << YN(IsWin2003()) << L"\n";
  std::wcout << L"IsWinVista:            " << YN(IsWinVista()) << L"\n";
  std::wcout << L"IsWin7:                " << YN(IsWin7()) << L"\n";
  std::wcout << L"IsWin8:                " << YN(IsWin8()) << L"\n";
  std::wcout << L"IsWin8_1:              " << YN(IsWin8_1()) << L"\n";
  std::wcout << L"IsWin10:               " << YN(IsWin10()) << L"\n";
  std::wcout << L"IsWin11:               " << YN(IsWin11()) << L"\n";
  std::wcout << L"IsWin(kWin10):         " << YN(IsWin(kWin10)) << L"\n";
  std::wcout << L"IsAtLeast(kWinXP):     " << YN(IsAtLeast(kWinXP)) << L"\n";
  std::wcout << L"IsAtMost(kWinVista):   " << YN(IsAtMost(kWinVista)) << L"\n";
  std::wcout << L"IsWinNewerThan(kWin7): " << YN(IsWinNewerThan(kWin7)) << L"\n";
  std::wcout << L"IsWinOlderThan(kWin10):" << YN(IsWinOlderThan(kWin10)) << L"\n";

  std::wcout << L"\n-- CPU --\n";
  std::wcout << L"CpuVendor:             " << CpuVendor() << L"\n";
  std::wcout << L"CpuModel:              " << CpuModel() << L"\n";
  std::wcout << L"NumCpuCores:           " << NumCpuCores() << L"\n";
  std::wcout << L"IsCPU64BitCapable:     " << YN(IsCPU64BitCapable()) << L"\n";
  std::wcout << L"CanExecuteSSE:         " << YN(CanExecuteSSE()) << L"\n";
  std::wcout << L"CanExecuteAVX:         " << YN(CanExecuteAVX()) << L"\n";
  std::wcout << L"CanExecuteAVX512:      " << YN(CanExecuteAVX512()) << L"\n";
  std::wcout << L"CpuFreqAvg:            " << CpuFreqAvg() << L" MHz.\n";
  std::wcout << L"CpuFreqMax:            " << CpuFreqMax() << L" MHz.\n";

  std::wcout << L"\n-- CPUID_INFO --\n";
  CPUID_INFO cpu_info = {};
  if (GetCPUInfo(&cpu_info)) {
    std::wcout << L"GetCPUInfo.raw_model:  " << cpu_info.raw_model << L"\n";
    std::wcout << L"GetCPUInfo.num_cores:  " << cpu_info.num_cores << L"\n";
  }

  std::wcout << std::endl;
  system("pause");
  return 0;
}
