#ifndef TEST_OSINFO_VERSION_H_
#define TEST_OSINFO_VERSION_H_

// We need to define _UNICODE and UNICODE for TCHAR
#ifndef UNICODE
 #define UNICODE
#endif

#ifndef _UNICODE
 #define _UNICODE
#endif

#if defined(__clang__) && defined(_UNICODE)
 #pragma code_page(65001) // UTF-8
#endif

#include "winsdkver.h"

#ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x0500 // Windows 2000
#endif

#ifndef WINVER
 #define WINVER 0x0500 // Same as _WIN32_WINNT above
#endif

#ifndef _WIN64_WINNT
 #define _WIN64_WINNT 0x0502 // Minimum version for 64 bit, Windows Server 2003
#endif

#ifndef _WIN32_IE
 #define _WIN32_IE 0x0501 // Minimum Internet Explorer version for common controls
#endif

#if _WIN32_WINNT < 0x0601 // If we are less than Windows 7, use old ATL for safety
 #ifndef _ATL_XP_TARGETING
  #define _ATL_XP_TARGETING // For using XP-compatible ATL/MFC functions
 #endif
#endif

#include "sdkddkver.h"

// clang-format off: Version DEFINES left alone

// These next few lines are where we control version number and copyright year
// Adhere to semver -> semver.org
#define MAJOR_VERSION 0
#define MINOR_VERSION 0
#define BUILD_VERSION 1

// Macro to convert to string
#if !defined(_STRINGIZER_)
 #define _STRINGIZER_
 #define _STRINGIZER(in) #in
 #define STRINGIZE(in) _STRINGIZER(in)
  // Wide-string variant: L ## "x" -> L"x". Two levels so the argument expands
 // before the L## paste widens the resulting narrow literal.
 #define _WIDEN(in) L ## in
 #define WIDEN(in) _WIDEN(in)
#endif // !defined(_STRINGIZER_)

// Main version constant
#ifndef _VERSION
 // Run stringizer above
 #define _VERSION(major,minor,build) WIDEN(STRINGIZE(major.minor.build))
#endif // _VERSION

// String constants
#define VERSION_STRING _VERSION(MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION)

#define COMMENTS        L"https://github.com/Alex313031/osinfo" // Project GitHub URL
#define COMPANYNAME     L"Alex313031" // My developer name
#define INTERNAL_NAME   L"test_osinfo"
#define TRADEMARKS      L"BSD-3" // License
#define LEGAL_COPYRIGHT L"\251 2026 Alex313031" // \251 is the © symbol

#ifdef TEST_OSINFO_STATIC
 #define APP_NAME        L"Test OSInfo (static)" // Title for console window
 #define FILE_DESCRIPT   L"OSInfo Library Test (Static)" // File description
 #define ORIG_FILENAME   L"test_osinfo_static.exe"
 #define PRODUCT_NAME    L"OSInfo Static Test"
#else
 #define APP_NAME        L"Test OSInfo (shared)"
 #define FILE_DESCRIPT   L"OSInfo Library Test (Shared)"
 #define ORIG_FILENAME   L"test_osinfo_shared.exe"
 #define PRODUCT_NAME    L"OSInfo Shared Test"
#endif // TEST_OSINFO_SHARED

#ifndef VS_FF_RELEASE
 #define VS_FF_RELEASE 0x00000000L
#endif

// clang-format on: Done with version DEFINES

#endif // TEST_OSINFO_VERSION_H_
