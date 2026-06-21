// Copyright (c) 2026 Alex313031

// Main functions of the OSInfo library, compiles to either a static lib or shared .dll

#include "os_info.h"

#include "cpu.h"

unsigned long WinVer          = 0UL;
unsigned long long WinVerFull = 0ULL;

unsigned long RealWinVer          = 0UL;
unsigned long long RealWinVerFull = 0ULL;

bool is_winnt = false;
bool is_win2k = false;
bool is_winxp = false;
bool is_win03 = false;
bool is_win06 = false;
bool is_win7  = false;
bool is_win8  = false;
bool is_win81 = false;
bool is_win10 = false;
bool is_win11 = false;

// Spoofable (honor the per-EXE Application Compatibility tab on XP+). Defined for
// both build modes - os_info.h declares these extern unconditionally and they are
// used by code present in both the .dll and the static lib.
ULONG NT_MAJOR = 0UL;
ULONG NT_MINOR = 0UL;
ULONG NT_BUILD = 0UL;
wchar_t NT_CSD_VERSION[kCsdVersionLen]{};
USHORT NT_SP_MAJOR = 0u;
USHORT NT_SP_MINOR = 0u;
USHORT NT_SUITE    = 0u;
UCHAR NT_TYPE      = 0u;
// Non-Spoofable
ULONG REAL_NT_MAJOR = 0UL;
ULONG REAL_NT_MINOR = 0UL;
ULONG REAL_NT_BUILD = 0UL;
// Our own generated strings
std::string NT_SERVICE_PACK    = "";
std::string NT_FEATURE_VERSION = "";
std::string NT_POST_STRING     = "";

// Statics (file-local state)
static bool is_initialized = false;

// Genuine Windows 11 starts at NT build 22000; anything below that reporting NT
// 10.0 is real Windows 10. Used to disambiguate the two, which share an NT version.
static constexpr ULONG kWin11MinBuild = 22000;

#ifndef OSINFO_STATIC_LIB
HINSTANCE gHinstDLL = nullptr;

static bool was_static_load = false;

// Main entry point when loading/unloading .DLL from address space of another process/thread.
// MUST have exact function signature BOOL WINAPI. DLL equivalent of WinMain.
// See https://www.transmissionzero.co.uk/computing/advanced-mingw-dll-topics/
OSINFO_API BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved) {
  gHinstDLL = hInstDLL;

  switch (dwReason) {
    // Called on LoadLibrary or import
    case DLL_PROCESS_ATTACH: {
      // Non-NULL means dll was statically imported.
      if (lpvReserved != nullptr) {
        was_static_load = true;
      }
      return InitOsInfoDll();
    }
    // Called when threads are run by parent process after DLL_PROCESS_ATTACH
    case DLL_THREAD_ATTACH: {
      if (is_initialized) {
        return TRUE; // Skip unnecessary call to InitOsInfoDll
      } else {
        return InitOsInfoDll();
      }
    }
    // Called when FreeLibrary is called or program shutting down
    case DLL_PROCESS_DETACH: {
      // If lpvReserved is NULL, DLL load failed or FreeLibrary was called.
      if (lpvReserved == nullptr) {
        return DeInitOsInfoDLL();
      } else {
        // Process is terminating
        return TRUE;
      }
    }
    case DLL_THREAD_DETACH: {
      return TRUE; // Unused in osinfo.dll
    }
    default:
      return FALSE;
  }
}

OSINFO_API HRESULT __cdecl DllGetVersion(DLLVERSIONINFO* pdvi) {
  if (pdvi == nullptr) {
    return E_POINTER;
  }

  switch (pdvi->cbSize) {
    case sizeof(DLLVERSIONINFO2): {
      // Caller passed DLLVERSIONINFO2 - fill extended fields
      DLLVERSIONINFO2* pdvi2 = reinterpret_cast<DLLVERSIONINFO2*>(pdvi);
      pdvi2->dwFlags         = 0;
      pdvi2->ullVersion      = MAKEDLLVERULL(MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION, 0);
    } // no break, fall through for backward compatibility DLLVERSIONINFO
    case sizeof(DLLVERSIONINFO): {
      // Fill base DLLVERSIONINFO fields
      pdvi->dwMajorVersion = MAJOR_VERSION;
      pdvi->dwMinorVersion = MINOR_VERSION;
      pdvi->dwBuildNumber  = BUILD_VERSION;
      pdvi->dwPlatformID   = DLLVER_PLATFORM_NT;
      return S_OK;
    }
    default: {
      // cbSize doesn't match a known structure (e.g. too small); report bad
      // argument rather than throwing an exception across the DLL boundary.
      return E_INVALIDARG;
    }
  }
}

static bool __cdecl InitOsInfoDll() {
  if (!is_initialized) {
    is_initialized = GetWinNTVersion() && GetRealWinNTVersion();
  }
  return is_initialized && (gHinstDLL != nullptr);
}

static bool __cdecl DeInitOsInfoDLL() {
  // TODO add memory cleanup here.
  WinVer         = 0UL;
  WinVerFull     = 0ULL;
  RealWinVer     = 0UL;
  RealWinVerFull = 0ULL;
  return (WinVer == 0UL && WinVerFull == 0ULL && RealWinVer == 0UL && RealWinVerFull == 0ULL);
}
#endif // !OSINFO_STATIC_LIB

static bool __cdecl EnsureInitialized() {
  // Initialize exactly once. The magic-static guard is thread-safe, so the static
  // lib's lazy first-use init cannot race; in the .dll, DllMain has normally run
  // InitOsInfoDll() already (under the loader lock).
  static const bool initialized = [] {
#ifndef OSINFO_STATIC_LIB
    if (!is_initialized) {
      OutputDebugStringW(
          L"OSInfo: EnsureInitialized() ran before DllMain init; initializing now.\n");
    }
#endif // !OSINFO_STATIC_LIB
    if (!is_initialized) {
      is_initialized = GetWinNTVersion() && GetRealWinNTVersion();
    }
    if (!is_initialized) {
      OutputDebugStringW(L"OSInfo: Failed to set Windows version bools!\n");
    }
    return is_initialized;
  }();
  return initialized;
}

// Packs two version parts into one value: (high << 8) | (low & 0xFF), e.g. (6, 1) -> 0x601.
unsigned long __cdecl combineToHex(unsigned long high, unsigned long low) {
  return (high << 8) | (low & 0xFF);
}

// Packs major/minor/build into the full NT version layout the public getters
// return: 0xMMmmBBBB (major<<24 | minor<<16 | low 16 bits of build).
static unsigned long long __cdecl PackNtVerFull(ULONG major, ULONG minor, ULONG build) {
  return (static_cast<unsigned long long>(major) << 24) |
         (static_cast<unsigned long long>(minor) << 16) |
         (static_cast<unsigned long long>(build) & 0xFFFFULL);
}

// Writes the three optional out-params when non-null. Shared by the fallback
// chain in GetRealVersions below so each tier doesn't repeat the null checks.
static void __cdecl WriteVersionOut(ULONG* major,
                                    ULONG* minor,
                                    ULONG* build,
                                    ULONG major_val,
                                    ULONG minor_val,
                                    ULONG build_val) {
  if (major != nullptr) {
    *major = major_val;
  }
  if (minor != nullptr) {
    *minor = minor_val;
  }
  if (build != nullptr) {
    *build = build_val;
  }
}

// Gets the real, unspoofable NT version number, for GetRealNTVer and internal
// comparison. Tries, in order, until one succeeds:
//   1. RtlGetNtVersionNumbers (XP+, undocumented) - ignores all compat shims.
//   2. RtlGetVersion (Win2000+, NT-native) - also unshimmed, returns the truth.
//   3. GetVersionExW (NT 4.0+) - last resort, only reached when the ntdll
//      functions are absent (i.e. NT 4.0), where no version shims exist anyway.
static bool __cdecl GetRealVersions(ULONG* major, ULONG* minor, ULONG* build) {
  static HMODULE hNtDll = GetModuleHandleW(kNtDllDll);

  if (hNtDll != nullptr) {
    // 1. RtlGetNtVersionNumbers packs a build-type flag into the top 4 bits of
    //    buildVer - mask them off so callers see the plain build number (e.g.
    //    2600 for XP SP3, 19045 for Win10 22H2).
    using RtlGetNtVersionNumbers_t = void(WINAPI*)(DWORD*, DWORD*, DWORD*);
    static RtlGetNtVersionNumbers_t pfnRtlGetNtVersionNumbers =
        reinterpret_cast<RtlGetNtVersionNumbers_t>(
            GetProcAddress(hNtDll, "RtlGetNtVersionNumbers"));
    if (pfnRtlGetNtVersionNumbers != nullptr) {
      DWORD majorVer = 0, minorVer = 0, buildVer = 0;
      pfnRtlGetNtVersionNumbers(&majorVer, &minorVer, &buildVer);
      if (majorVer != 0) {
        WriteVersionOut(major, minor, build, majorVer, minorVer, buildVer & 0x0FFFFFFFu);
        return true;
      }
    }

    // 2. RtlGetVersion (Win2K+, documented NT-native, not shimmed by
    //    application-compatibility manifests unlike GetVersionExW on Win8.1+).
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    static RtlGetVersion_t pfnRtlGetVersion =
        reinterpret_cast<RtlGetVersion_t>(GetProcAddress(hNtDll, "RtlGetVersion"));
    if (pfnRtlGetVersion != nullptr) {
      OSVERSIONINFOW version_info      = {};
      version_info.dwOSVersionInfoSize = sizeof(version_info);
      if (pfnRtlGetVersion(&version_info) == STATUS_SUCCESS && version_info.dwMajorVersion != 0) {
        WriteVersionOut(major, minor, build, version_info.dwMajorVersion,
                        version_info.dwMinorVersion, version_info.dwBuildNumber);
        return true;
      }
    }
  }

  // 3. Last resort for NT 4.0 (no RtlGetVersion/RtlGetNtVersionNumbers). Subject
  //    to app-compat shims on Win8.1+, but this branch is only reached when those
  //    ntdll functions are missing, where the shim infrastructure doesn't exist.
  OSVERSIONINFOW version_info      = {};
  version_info.dwOSVersionInfoSize = sizeof(version_info);
  if (GetVersionExW(&version_info) != 0 && version_info.dwMajorVersion != 0) {
    WriteVersionOut(major, minor, build, version_info.dwMajorVersion, version_info.dwMinorVersion,
                    version_info.dwBuildNumber);
    return true;
  }

  return false;
}

static bool __cdecl GetRealWinNTVersion() {
  bool success = false;
  ULONG major = 0UL, minor = 0UL, build = 0UL;
  if (!GetRealVersions(&major, &minor, &build)) {
    success = false;
  } else {
    REAL_NT_MAJOR  = major;
    REAL_NT_MINOR  = minor;
    REAL_NT_BUILD  = build;
    RealWinVer     = combineToHex(REAL_NT_MAJOR, REAL_NT_MINOR);
    RealWinVerFull = PackNtVerFull(REAL_NT_MAJOR, REAL_NT_MINOR, REAL_NT_BUILD);
    if (RealWinVer > 0UL) {
      switch (RealWinVer) {
        case NTVER_40:
          is_winnt = true;
          break;
        case NTVER_2K:
          is_win2k = true;
          break;
        case NTVER_XP:
          is_winxp = true;
          break;
        case NTVER_2K3:
          is_win03 = true;
          break;
        case NTVER_VISTA:
          is_win06 = true;
          break;
        case NTVER_7:
          is_win7 = true;
          break;
        case NTVER_8:
          is_win8 = true;
          break;
        case NTVER_81:
          is_win81 = true;
          break;
        case NTVER_10: {
          // Win11 RTM is build 22000; below that, NTVER_10 is genuine Windows 10.
          // Use the unspoofable REAL_NT_BUILD here, not the shimmable NT_BUILD.
          if (REAL_NT_BUILD >= kWin11MinBuild) {
            is_win11 = true;
          } else {
            is_win10 = true;
          }
        } break;
        default:
          // Unknown but valid (e.g. a future Windows) - don't classify, and do
          // NOT crash the host process; just report no specific version matched.
          return false;
      }
      success = true;
    }
  }
  return success;
}

static bool __cdecl GetWinNTVersion() {
  bool success = false;
  // Use RtlGetVersion from winnt.h, not wdm.h
  NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
  // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexw
  OSVERSIONINFOEXW osInfo = {};

  // Make sure we get the dll
  HMODULE NtDllModule = GetModuleHandleW(kNtDllDll);

  // Get and run ntdll.dll function pointer to RtlGetVersion()
  if (NtDllModule != nullptr) {
    FARPROC proc = GetProcAddress(NtDllModule, "RtlGetVersion");
    memcpy(&RtlGetVersion, &proc, sizeof(proc));
  } else {
    RtlGetVersion = nullptr;
  }

  osInfo.dwOSVersionInfoSize = sizeof osInfo;

  // Primary: RtlGetVersion (Win2000+). On XP+ it honors the per-EXE Application
  // Compatibility tab - exactly the "reported"/spoofable value we want here (the
  // Real path bypasses it). On NT 4.0 RtlGetVersion doesn't exist, so fall back
  // to GetVersionExW; NT4 has no compat version shimming, so it's the truth there.
  bool got = (RtlGetVersion != nullptr && RtlGetVersion(&osInfo) == STATUS_SUCCESS &&
              osInfo.dwMajorVersion != 0);
  if (!got) {
    osInfo                     = {};
    osInfo.dwOSVersionInfoSize = sizeof osInfo;
    got                        = (GetVersionExW(reinterpret_cast<LPOSVERSIONINFOW>(&osInfo)) != 0 &&
                                  osInfo.dwMajorVersion != 0);
  }

  // Only trust osInfo if one of the calls above succeeded; otherwise the fields
  // are garbage.
  if (got) {
    NT_MAJOR = osInfo.dwMajorVersion;
    NT_MINOR = osInfo.dwMinorVersion;
    NT_BUILD = osInfo.dwBuildNumber;
#if _WIN32_WINNT <= 0x0500 || defined(__MINGW32__)
    // Windows 2000 and MinGW compatible string copy function
    lstrcpynW(NT_CSD_VERSION, osInfo.szCSDVersion, kCsdVersionLen);
#else
    // Safer Windows XP+ MSVS version
    wcscpy_s(NT_CSD_VERSION, osInfo.szCSDVersion);
#endif
    NT_SP_MAJOR = osInfo.wServicePackMajor;
    NT_SP_MINOR = osInfo.wServicePackMinor;
    NT_SUITE    = osInfo.wSuiteMask;
    NT_TYPE     = osInfo.wProductType;
    success     = true;
  } else {
    success = false;
  }

  if (!success) {
    OutputDebugStringW(L"OSInfo: Failed to get Windows version!\n");
  } else {
    // Set our extern WinVer. Compute WinVerFull inline rather than calling
    // GetReportedNTVer(), which routes through EnsureInitialized() and would recurse
    // back into GetWinNTVersion() before is_initialized has been set.
    WinVer     = combineToHex(NT_MAJOR, NT_MINOR);
    WinVerFull = PackNtVerFull(NT_MAJOR, NT_MINOR, NT_BUILD);
  }

  return success;
}

// Builds the dotted NT version string "major.minor.build", e.g. "5.1.2600".
static std::string const __cdecl GetNTString() {
  // NT version number
  std::string NtVer;
  if (!EnsureInitialized()) {
    return std::string();
  }

  // Make sure that the values returned make sense.
  // NT Kernels with numbers outside this range don't exist
  if (REAL_NT_MAJOR >= 3 && REAL_NT_MAJOR <= 11) {
    // Define NtVer as human readable string literal separated by decimals
    NtVer = std::to_string(REAL_NT_MAJOR) + "." + std::to_string(REAL_NT_MINOR) + "." +
            std::to_string(REAL_NT_BUILD);
  }

  return NtVer;
}

// Use WINNT API functions to get OS and system information, this can be spoofed.
OSINFO_API std::string const __cdecl GetOSNameA() {
  // Human readable OS name
  std::string OsVer;
  // For obscure versions or pre NT4 SP6
  std::ostringstream debugStream;

  if (!EnsureInitialized()) {
    return std::string();
  }

  // Not every version branch below assigns NT_FEATURE_VERSION, so clear it up front
  // rather than letting a prior call's value bleed into NT_POST_STRING.
  NT_FEATURE_VERSION = "";

  // Get the service pack
  const bool fallback = REAL_NT_MAJOR < 6;
  std::wstring NT_SP;
  if (fallback) {
    if (NT_SP_MINOR == 0) {
      NT_SP = L"Service Pack " + std::to_wstring(NT_SP_MAJOR);
    } else {
      NT_SP = L"Service Pack " + std::to_wstring(NT_SP_MAJOR) + L"." + std::to_wstring(NT_SP_MINOR);
    }
  } else {
    std::wstring csd_version(NT_CSD_VERSION);
    NT_SP = csd_version;
  }
  NT_SERVICE_PACK = WstringToString(NT_SP);

  if (NT_MAJOR <= 3) {
    NT_FEATURE_VERSION = NT_SERVICE_PACK;
  }
  if (NT_MAJOR == 4) {
    if (NT_BUILD < 1381) {
      if (NT_SUITE == VER_SUITE_TERMINAL) {
        NT_FEATURE_VERSION = " Beta Cairo Build";
      } else {
        NT_FEATURE_VERSION = " Beta Hydra Build";
      }
    } else {
      if (NT_SUITE == VER_SUITE_TERMINAL) {
        NT_FEATURE_VERSION = " (Cairo)";
      } else {
        // NT_FEATURE_VERSION = " (Hydra)";
      }
    }
  } else if (NT_MAJOR == 5) {
    switch (NT_MINOR) {
      case 0:
        if (NT_BUILD > 1386 && NT_BUILD < 2195) {
          NT_FEATURE_VERSION = " Beta Win2K Build";
        }
        break;
      case 1:
        if (NT_BUILD > 2196 && NT_BUILD < 2600) {
          NT_FEATURE_VERSION = " Beta Whistler Build";
        }
        if (NT_BUILD == 2700) {
          NT_FEATURE_VERSION = " MCE 2005";
        }
        if (NT_BUILD == 2710) {
          NT_FEATURE_VERSION = " MCE 2005 Update Rollup 2";
        }
        break;
      case 2:
        if (NT_BUILD > 2228 && NT_BUILD < 3790) {
          NT_FEATURE_VERSION = " Beta Whistler Build";
        }
        if (NT_SUITE == VER_SUITE_WH_SERVER) {
          if (NT_BUILD > 1282 && NT_BUILD < 3790) {
            NT_FEATURE_VERSION = " Beta Quattro Build";
          }
        }
        break;
      case 5:
        NT_FEATURE_VERSION = " (Neptune)";
        break;
      default:
        NT_FEATURE_VERSION = NT_SERVICE_PACK;
        break;
    }
  } else if (NT_MAJOR == 6) {
    switch (NT_MINOR) {
      case 0:
        if (NT_BUILD > 3663 && NT_BUILD < 6000) {
          NT_FEATURE_VERSION = " Beta Longhorn Build";
        } else if (NT_BUILD == 6003) {
          NT_FEATURE_VERSION = " KB4489887";
        }
        break;
      case 1:
        if (NT_BUILD > 6429 && NT_BUILD < 7600) {
          NT_FEATURE_VERSION = " Beta Win7 Build";
        }
        break;
      case 2:
        if (NT_BUILD > 7652 && NT_BUILD < 9200) {
          NT_FEATURE_VERSION = " Beta Win8 Build";
        }
        break;
      case 3:
        if (NT_BUILD > 9255 && NT_BUILD < 9600) {
          NT_FEATURE_VERSION = " Beta Blue Build";
        }
        break;
      default:
        NT_FEATURE_VERSION = NT_SERVICE_PACK;
        break;
    }
    // No such thing as feature releases for Windows 8.1 and below
  } else if (NT_MAJOR >= 10) {
    if (NT_BUILD < 10240) {
      NT_FEATURE_VERSION = "Beta Threshold Build ";
    } else if (NT_BUILD == 10240) {
      NT_FEATURE_VERSION = "1507 (RTM 2015 Release) ";
    } else if (NT_BUILD == 10586) {
      NT_FEATURE_VERSION = "1511 (Nov. 2015 Update) ";
    } else if (NT_BUILD == 14393) {
      NT_FEATURE_VERSION = "1607 (Anniversary Update) ";
    } else if (NT_BUILD == 15063) {
      NT_FEATURE_VERSION = "1703 (Creators Update) ";
    } else if (NT_BUILD == 16299) {
      NT_FEATURE_VERSION = "1709 (Fall Creators Update) ";
    } else if (NT_BUILD == 17134) {
      NT_FEATURE_VERSION = "1803 (Apr. 2018 Update) ";
    } else if (NT_BUILD == 17763) {
      NT_FEATURE_VERSION = "1809 (Oct. 2018 Update) ";
    } else if (NT_BUILD == 18362) {
      NT_FEATURE_VERSION = "1903 (May. 2019 Update) ";
    } else if (NT_BUILD == 18363) {
      NT_FEATURE_VERSION = "1909 (Nov. 2019 Update) ";
    } else if (NT_BUILD == 19041) {
      NT_FEATURE_VERSION = "2004 (May. 2020 Update) ";
    } else if (NT_BUILD == 19042) {
      NT_FEATURE_VERSION = "20H2 (Oct. 2020 Update) ";
    } else if (NT_BUILD == 19043) {
      NT_FEATURE_VERSION = "21H1 (May. 2021 Update) ";
    } else if (NT_BUILD == 19044) {
      NT_FEATURE_VERSION = "21H2 (Nov. 2021 Update) ";
    } else if (NT_BUILD == 19045) {
      NT_FEATURE_VERSION = "22H2 (Oct. 2022 Update) ";
    } else if (NT_BUILD == 20348) {
      NT_FEATURE_VERSION = "21H2 (Aug. 2021 Update) ";
    } else if (NT_BUILD == 25398) {
      NT_FEATURE_VERSION = "23H2 (Oct. 2023 Update) ";
    } else if (NT_BUILD < 22000 && NT_BUILD > 20348 && NT_BUILD != 25398) {
      NT_FEATURE_VERSION = "Beta Build ";
    } else if (NT_BUILD == 22000) {
      NT_FEATURE_VERSION = "21H2 (Sun Valley RTM 2021 Release) ";
    } else if (NT_BUILD == 22621) {
      NT_FEATURE_VERSION = "22H2 (Sun Valley 2 Sep. 2022 Update) ";
    } else if (NT_BUILD == 22631) {
      NT_FEATURE_VERSION = "23H2 (Sun Valley 3 Oct. 2023 Update) ";
    } else if (NT_BUILD == 26100) {
      NT_FEATURE_VERSION = "24H2 (Hudson Valley Oct. 2024 Update) ";
    } else if (NT_BUILD == 26200) {
      NT_FEATURE_VERSION = "25H2 (Hudson Valley 2 Sep. 2025 Update) ";
    } else if (NT_BUILD >= 28000) {
      NT_FEATURE_VERSION = "26H1 (2026 Update) ";
    } else {
      NT_FEATURE_VERSION = "(Build " + std::to_string(NT_BUILD) + ") ";
    }
  }
  NT_POST_STRING = NT_SERVICE_PACK + NT_FEATURE_VERSION;

  if (NT_MAJOR == 3) {
    OsVer = "Windows NT 3.x";
  } else if (NT_MAJOR == 4) {
    // Known to be buggy on NT 4.0, and needs SP6 to work
    switch (NT_MINOR) {
      case 0:
        if (NT_TYPE == VER_NT_WORKSTATION) {
          OsVer = "Windows NT 4.0 Workstation ";
        } else {
          if (NT_SUITE == VER_SUITE_TERMINAL) {
            OsVer = "Windows NT 4.0 Terminal Server ";
          } else if (NT_SUITE == VER_SUITE_ENTERPRISE) {
            OsVer = "Windows NT 4.0 Enterprise Server ";
          } else {
            OsVer = "Windows NT 4.0 Server ";
          }
        }
        OsVer += NT_POST_STRING;
        break;
      default:
        debugStream.str("");
        debugStream.clear();
        debugStream << " Unknown Windows " << NT_MAJOR << "." << NT_MINOR << "." << NT_BUILD
                    << std::endl;
        OsVer = debugStream.str();
        break;
    }
  } else if (NT_MAJOR == 5) {
    switch (NT_MINOR) {
      case 0:
        if (NT_TYPE == VER_NT_WORKSTATION) {
          OsVer = "Windows 2000 Professional " + NT_POST_STRING;
        } else {
          if (NT_SUITE == VER_SUITE_ENTERPRISE) {
            OsVer = "Windows 2000 Advanced Server " + NT_POST_STRING;
          } else if (NT_SUITE == VER_SUITE_DATACENTER) {
            OsVer = "Windows 2000 Datacenter " + NT_POST_STRING;
          } else {
            OsVer = "Windows 2000 Server " + NT_POST_STRING;
          }
        }
        break;
      case 1:
        if (GetSystemMetrics(SM_STARTER) != 0) {
          OsVer = "Windows XP Starter Edition " + NT_POST_STRING;
        } else if (GetSystemMetrics(SM_TABLETPC) != 0) {
          OsVer = "Windows XP Tablet PC Edition " + NT_POST_STRING;
        } else {
          if (NT_SUITE == VER_SUITE_PERSONAL) {
            OsVer = "Windows XP Home Edition " + NT_POST_STRING;
          } else if (NT_SUITE == VER_SUITE_EMBEDDEDNT) {
            OsVer = "Windows XP Embedded " + NT_POST_STRING;
          } else {
            OsVer = "Windows XP Professional " + NT_POST_STRING;
          }
        }
        break;
      case 2:
        if (NT_SUITE == VER_SUITE_WH_SERVER) {
          OsVer = "Windows Home Server " + NT_POST_STRING;
        } else {
          if (NT_TYPE == VER_NT_WORKSTATION) {
            OsVer = "XP x64 " + NT_POST_STRING;
          } else {
            if (GetSystemMetrics(SM_SERVERR2) == 0) {
              if (NT_SUITE == VER_SUITE_BLADE) {
                OsVer = "Windows Server 2003 Web Edition " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_COMPUTE_SERVER) {
                OsVer = "Windows Server 2003 Compute Cluster Edition " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_STORAGE_SERVER) {
                OsVer = "Windows Server 2003 Enterprise " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_ENTERPRISE) {
                OsVer = "Windows Server 2003 Enterprise " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_DATACENTER) {
                OsVer = "Windows Server 2003 Datacenter " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_SMALLBUSINESS) {
                OsVer = "Windows Server 2003 Small Business Server " + NT_POST_STRING;
              } else {
                OsVer = "Windows Server 2003 Standard " + NT_POST_STRING;
              }
            } else {
              if (NT_SUITE == VER_SUITE_BLADE) {
                OsVer = "Windows Server 2003 R2 Web Edition " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_COMPUTE_SERVER) {
                OsVer = "Windows Server 2003 R2 Compute Cluster Edition " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_STORAGE_SERVER) {
                OsVer = "Windows Server 2003 R2 Enterprise " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_ENTERPRISE) {
                OsVer = "Windows Server 2003 R2 Enterprise " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_DATACENTER) {
                OsVer = "Windows Server 2003 R2 Datacenter " + NT_POST_STRING;
              } else if (NT_SUITE == VER_SUITE_SMALLBUSINESS) {
                OsVer = "Windows Server 2003 R2 Small Business Server " + NT_POST_STRING;
              } else {
                OsVer = "Windows Server 2003 R2 Standard " + NT_POST_STRING;
              }
            }
          }
        }
        break;
      case 5:
        OsVer = "Windows Neptune " + NT_POST_STRING;
        break;
      default:
        debugStream.str("");
        debugStream.clear();
        debugStream << " Unknown Windows " << NT_MAJOR << "." << NT_MINOR << "." << NT_BUILD
                    << std::endl;
        OsVer = debugStream.str();
        break;
    }
  } else if (NT_MAJOR == 6) {
    switch (NT_MINOR) {
      case 0:
        if (NT_TYPE == VER_NT_WORKSTATION) {
          if (GetSystemMetrics(SM_STARTER) != 0) {
            OsVer = "Windows Vista Starter " + NT_POST_STRING;
          } else {
            if (NT_SUITE == VER_SUITE_PERSONAL) {
              OsVer = "Windows Vista Home " + NT_POST_STRING;
            } else {
              OsVer = "Windows Vista " + NT_POST_STRING;
            }
          }
        } else {
          if (NT_SUITE == VER_SUITE_ENTERPRISE) {
            OsVer = "Windows Server 2008 Enterprise " + NT_POST_STRING;
          } else if (NT_SUITE == VER_SUITE_DATACENTER) {
            OsVer = "Windows Server 2008 Datacenter " + NT_POST_STRING;
          } else {
            OsVer = "Windows Server 2008 " + NT_POST_STRING;
          }
        }
        break;
      case 1:
        if (NT_BUILD == 8400 || NT_SUITE == VER_SUITE_WH_SERVER) {
          OsVer = "Windows Home Server 2011 " + NT_POST_STRING;
        } else {
          if (NT_TYPE == VER_NT_WORKSTATION) {
            if (GetSystemMetrics(SM_STARTER) != 0) {
              OsVer = "Windows 7 Starter " + NT_POST_STRING;
            } else {
              if (NT_SUITE == VER_SUITE_PERSONAL) {
                OsVer = "Windows 7 Home " + NT_POST_STRING;
              } else {
                OsVer = "Windows 7 " + NT_POST_STRING;
              }
            }
          } else {
            if (NT_SUITE == VER_SUITE_ENTERPRISE) {
              OsVer = "Windows Server 2008 R2 Enterprise " + NT_POST_STRING;
            } else if (NT_SUITE == VER_SUITE_DATACENTER) {
              OsVer = "Windows Server 2008 R2 Datacenter " + NT_POST_STRING;
            } else {
              OsVer = "Windows Server 2008 R2 " + NT_POST_STRING;
            }
          }
        }
        break;
      case 2:
        if (NT_TYPE == VER_NT_WORKSTATION) {
          OsVer = "Windows 8 " + NT_POST_STRING;
        } else {
          OsVer = "Windows Server 2012 " + NT_POST_STRING;
        }
        break;
      case 3:
        if (NT_TYPE == VER_NT_WORKSTATION) {
          OsVer = "Windows 8.1 " + NT_POST_STRING;
        } else {
          OsVer = "Windows Server 2012 R2 " + NT_POST_STRING;
        }
        break;
      default:
        debugStream.str("");
        debugStream.clear();
        debugStream << " Unknown Windows " << NT_MAJOR << "." << NT_MINOR << "." << NT_BUILD
                    << std::endl;
        OsVer = debugStream.str();
        break;
    }
  } else if (NT_MAJOR == 10) {
    switch (NT_MINOR) {
      case 0: {
        if (!is_win11) {
          if (NT_TYPE == VER_NT_WORKSTATION) {
            OsVer = "Windows 10 " + NT_POST_STRING;
          } else {
            if (NT_BUILD >= 14393 && NT_BUILD < 17763) {
              OsVer = "Windows Server 2016 " + NT_POST_STRING;
            } else if (NT_BUILD >= 17763 && NT_BUILD < 20348) {
              OsVer = "Windows Server 2019 " + NT_POST_STRING;
            } else if (NT_BUILD >= 20348 && NT_BUILD < 25398) {
              OsVer = "Windows Server 2022 " + NT_POST_STRING;
            } else {
              OsVer = "Windows Server " + NT_POST_STRING;
            }
          }
        } else {
          if (NT_TYPE == VER_NT_WORKSTATION) {
            OsVer = "Windows 11 " + NT_POST_STRING;
          } else {
            if (NT_BUILD == 26100) {
              OsVer = "Windows Server 2025 " + NT_POST_STRING;
            } else {
              OsVer = "Windows Server " + NT_POST_STRING;
            }
          }
        }
        break;
      }
      case 1:
        OsVer = "Windows 12 " + NT_POST_STRING;
        break;
      default:
        debugStream.str("");
        debugStream.clear();
        debugStream << " - Unknown Windows " << NT_MAJOR << "." << NT_MINOR << "." << NT_BUILD
                    << std::endl;
        OsVer = debugStream.str();
        break;
    }
  } else {
    debugStream.str("");
    debugStream.clear();
    debugStream << "NT_MAJOR out of bounds!";
    OsVer = debugStream.str();
  }

  return OsVer;
}

// This can be spoofed.
OSINFO_API std::wstring const __cdecl GetOSNameW() {
  return StringToWstring(GetOSNameA());
}

OSINFO_API std::string const __cdecl GetWinVersionA() {
  const std::string ver = GetNTString();
  return ver;
}

OSINFO_API std::wstring const __cdecl GetWinVersionW() {
  return StringToWstring(GetWinVersionA());
}

OSINFO_API unsigned long long const __cdecl GetReportedNTVer() {
  if (!EnsureInitialized()) {
    return 0ULL;
  }
  const unsigned long long retval = PackNtVerFull(NT_MAJOR, NT_MINOR, NT_BUILD);
  return retval;
}

OSINFO_API unsigned long long const __cdecl GetRealNTVer() {
  if (!EnsureInitialized()) {
    return 0ULL;
  }
  const unsigned long long retval = PackNtVerFull(REAL_NT_MAJOR, REAL_NT_MINOR, REAL_NT_BUILD);
  return retval;
}

OSINFO_API unsigned long const __cdecl GetReportedShortNTVer() {
  if (!EnsureInitialized()) {
    return 0UL;
  }
  const unsigned long retval = combineToHex(NT_MAJOR, NT_MINOR);
  return retval;
}

OSINFO_API unsigned long const __cdecl GetRealShortNTVer() {
  if (!EnsureInitialized()) {
    return 0UL;
  }
  const unsigned long retval = combineToHex(REAL_NT_MAJOR, REAL_NT_MINOR);
  return retval;
}

OSINFO_API int const __cdecl GetServicePackNumber() {
  if (!EnsureInitialized()) {
    return -1;
  }
  const int kSPMajor = static_cast<int>(NT_SP_MAJOR);
  return kSPMajor;
}

OSINFO_API std::string const __cdecl GetServicePackA() {
  return WstringToString(GetServicePackW());
}

OSINFO_API std::wstring const __cdecl GetServicePackW() {
  if (!EnsureInitialized()) {
    return std::wstring();
  }
  // Get the service pack
  const bool fallback = REAL_NT_MAJOR < 6;
  std::wstring service_pack;
  if (fallback) {
    if (NT_SP_MINOR == 0) {
      service_pack = L"Service Pack " + std::to_wstring(NT_SP_MAJOR);
    } else {
      service_pack =
          L"Service Pack " + std::to_wstring(NT_SP_MAJOR) + L"." + std::to_wstring(NT_SP_MINOR);
    }
  } else {
    std::wstring csd_version(NT_CSD_VERSION);
    service_pack = csd_version;
  }
  return service_pack;
}

// Converts a UTF-8 std::string to a UTF-16 std::wstring. Returns empty on
// conversion failure (never throws - this is reached from exported functions and
// an exception must not cross the DLL boundary).
static std::wstring const __cdecl StringToWstring(const std::string& str) {
  if (str.empty()) {
    return std::wstring();
  }
  // Get length of string
  int wide_len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
  if (wide_len == 0) {
    return std::wstring();
  }

  // Convert it!
  std::wstring result(wide_len, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], wide_len);

  // Remove the null terminator added by Windows API ugh
  if (!result.empty() && result.back() == L'\0') {
    result.pop_back();
  }
  return result;
}

// Converts a UTF-16 std::wstring to a UTF-8 std::string. Returns empty on
// conversion failure (never throws - reached from exported functions, so an
// exception must not cross the DLL boundary).
static std::string const __cdecl WstringToString(const std::wstring& wstr) {
  if (wstr.empty()) {
    return std::string();
  }
  int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);

  if (utf8_len == 0) {
    return std::string();
  }

  // Convert
  std::string result(utf8_len, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], utf8_len, nullptr, nullptr);

  // Remove trailing null terminator
  if (!result.empty() && result.back() == '\0') {
    result.pop_back();
  }
  return result;
}

// Intentionally execute an invalid opcode to kill the program and signal to debugger
// See http://ref.x86asm.net/coder32.html
static inline void __cdecl ImmediateDebugCrash() {
#ifdef __MINGW32__
  asm("int3\n\t"
      "ud2");
#else
  __asm int 3 // Execute int3 interrupt
      __asm {
    UD2
  } // Execute 0x0F, 0x0B
#endif // __MINGW32__
}

// Backs the NOTREACHED() macro: logs the function name, shows an error box, then crashes.
void __cdecl NotReachedImpl(const std::string& func_name) {
  // Log function name and then crash the program
  const std::string func_string = func_name;
  std::ostringstream failstream;
  failstream << "NOTREACHED(): " << func_string;
  const std::string fail_str   = failstream.str();
  const std::wstring error_msg = StringToWstring(fail_str);
  std::wcerr << error_msg << std::endl;
  OutputDebugStringW(error_msg.c_str());
  if (MessageBoxW(nullptr, error_msg.c_str(), kOsInfoError, MB_OK | MB_ICONERROR) != 0) {
    ImmediateDebugCrash();
  }
}

OSINFO_API std::string const __cdecl GetOsInfoVersionA() {
  if (!EnsureInitialized()) {
    return std::string();
  }
  return std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION) + "." +
         std::to_string(BUILD_VERSION);
}

OSINFO_API std::wstring const __cdecl GetOsInfoVersionW() {
  return StringToWstring(GetOsInfoVersionA());
}

OSINFO_API const bool __cdecl IsWinNT4() {
  EnsureInitialized();
  return is_winnt;
}
OSINFO_API const bool __cdecl IsWin2K() {
  EnsureInitialized();
  return is_win2k;
}
OSINFO_API const bool __cdecl IsWinXP() {
  EnsureInitialized();
  return is_winxp;
}
OSINFO_API const bool __cdecl IsWin2003() {
  EnsureInitialized();
  return is_win03;
}
OSINFO_API const bool __cdecl IsWinVista() {
  EnsureInitialized();
  return is_win06;
}
OSINFO_API const bool __cdecl IsWin7() {
  EnsureInitialized();
  return is_win7;
}
OSINFO_API const bool __cdecl IsWin8() {
  EnsureInitialized();
  return is_win8;
}
OSINFO_API const bool __cdecl IsWin8_1() {
  EnsureInitialized();
  return is_win81;
}
OSINFO_API const bool __cdecl IsWin10() {
  EnsureInitialized();
  return is_win10;
}
OSINFO_API const bool __cdecl IsWin11() {
  EnsureInitialized();
  return is_win11;
}

// Real short NT version for the comparison helpers below, with Win10/Win11
// disambiguated. Both report NT 10.0 (0x0A00), so a genuine Win11 (unspoofable
// build >= kWin11MinBuild) is lifted to the distinct kWin11 sentinel; every other
// version passes through as its raw RealWinVer. This is what lets IsWin(kWin11),
// IsAtMost(kWin10), etc. tell the two apart. Callers must have initialized first.
static unsigned long __cdecl EffectiveRealWinVer() {
  if (RealWinVer == NTVER_10 && REAL_NT_BUILD >= kWin11MinBuild) {
    return kWin11;
  }
  return RealWinVer;
}

OSINFO_API const bool __cdecl IsWin(const unsigned long check_ver) {
  EnsureInitialized();
  const bool is_ver = (EffectiveRealWinVer() == check_ver);
  return is_ver;
}

OSINFO_API const bool __cdecl IsAtLeast(const unsigned long check_ver) {
  EnsureInitialized();
  const bool is_at_least = (EffectiveRealWinVer() >= check_ver);
  return is_at_least;
}

OSINFO_API const bool __cdecl IsAtMost(const unsigned long check_ver) {
  EnsureInitialized();
  const bool is_at_most = (EffectiveRealWinVer() <= check_ver);
  return is_at_most;
}

OSINFO_API const bool __cdecl IsWinNewerThan(const unsigned long check_ver) {
  EnsureInitialized();
  const bool is_newer = (EffectiveRealWinVer() > check_ver);
  return is_newer;
}

OSINFO_API const bool __cdecl IsWinOlderThan(const unsigned long check_ver) {
  EnsureInitialized();
  const bool is_older = (EffectiveRealWinVer() < check_ver);
  return is_older;
}

OSINFO_API const bool __cdecl IsWoW64() {
  // IsWow64Process is XP+ only; on older systems WoW64 doesn't exist anyway.
  HMODULE hKernel32 = GetModuleHandleW(kKernel32Dll);
  if (IsWinOlderThan(NTVER_XP) || !hKernel32) {
    return false;
  }

  const IS_WOW64_PROCESS_ pIsWow64Process =
      reinterpret_cast<IS_WOW64_PROCESS_>(GetProcAddress(hKernel32, "IsWow64Process"));
  if (pIsWow64Process == nullptr) {
    return false;
  }

  BOOL isWoW64 = FALSE;
  if (!pIsWow64Process(GetCurrentProcess(), &isWoW64)) {
    return false;
  }
  return static_cast<bool>(isWoW64);
}
