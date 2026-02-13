#ifndef OSINFO_DLL_VERSION_H_
#define OSINFO_DLL_VERSION_H_

// We need to define _UNICODE and UNICODE for TCHAR
#ifndef UNICODE
 #define UNICODE
#endif

#ifndef _UNICODE
 #define _UNICODE
#endif

#if  defined(__clang__) && defined(_UNICODE)
 #pragma code_page(65001) // UTF-8
#endif // __clang__

// This file is for specifying the target windows version, as well as application
// version constants.

/* Including SDKDDKVer.h defines the highest available Windows platform.
   If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
   set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h. */

#ifndef __MINGW32__
 #include <WinSDKVer.h> // Doesn't exist in MinGW
#endif // __MINGW32__

#ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x0500 // Windows 2000
#endif // _WIN32_WINNT
#ifndef WINVER
 #define WINVER 0x0500 // Same as above
#endif // WINVER
#ifndef _WIN64_WINNT
 #define _WIN64_WINNT 0x0502 // Minimum version for 64 bit, Windows Server 2003
#endif // _WIN64_WINNT
#ifndef _WIN32_IE
 #define _WIN32_IE 0x0501 // Minimum Internet Explorer version for common controls
#endif // _WIN32_IE

#if _WIN32_WINNT <= 0x0600 // If we are less than Windows 7, use old ATL for safety
 #ifndef _ATL_XP_TARGETING
  #define _ATL_XP_TARGETING // For using XP-compatible ATL/MFC functions
 #endif // _ATL_XP_TARGETING
#endif // _WIN32_WINNT <= 0x0600

#ifndef __MINGW32__
 #include <SDKDDKVer.h> // Doesn't exist in MinGW
#endif // __MINGW32__

// Macro to convert to string
#if !defined(_STRINGIZER) && !defined(STRINGIZE)
 #define _STRINGIZER(in) #in
 #define STRINGIZE(in) _STRINGIZER(in)
#endif // !defined(_STRINGIZER) && !defined(STRINGIZE)

// Main version constant
#ifndef _VERSION
 // Run stringizer above
 #define _VERSION(major,minor,build) STRINGIZE(major) "." STRINGIZE(minor) "." STRINGIZE(build)
#endif // _VERSION

// These next few lines are where we control version number and copyright year
// Adhere to semver > semver.org
#define MAJOR_VERSION 1
#define MINOR_VERSION 5
#define BUILD_VERSION 2

#ifndef OSINFO_VERSION_STRING
 #define OSINFO_VERSION_STRING _VERSION(MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION)
 #define LEGAL_COPYRIGHT L"\251 2025-2026 Alex313031" // \251 is the © symbol
#endif // OSINFO_VERSION_STRING

#endif // OSINFO_DLL_VERSION_H_
