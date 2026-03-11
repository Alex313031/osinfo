#ifndef OSINFO_STDAFX_H_
#define OSINFO_STDAFX_H_

#include "./version.h"

// clang-format off
// Windows Header Files
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#ifdef _MSC_VER
 #define WIN32_NO_STATUS
#endif
#include <windows.h>  // Main Windows header
#ifdef _MSC_VER
 #undef WIN32_NO_STATUS
#endif
#include <winternl.h> // Main Windows header
#include <winver.h>   // Windows version functions
#include <commctrl.h> // Common controls header
#include <mmsystem.h> // For playing sounds
#include <ntstatus.h> // For full NTSTATUS codes (e.g. STATUS_SUCCESS)
#include <tchar.h>    // For TCHAR, and automatically deducing wchar_t type
// clang-format on

// C++ STL Headers
#include <cmath>     // For C-style math functions
#include <iomanip>   // std::setprecision
#include <iostream>  // std::cout std::wcout
#include <sstream>   // std::ostringstream std::wostringstream
#include <stdexcept> // exception handline
#include <string>    // std::string std::wstring

/* Defines handling */

#ifndef __FUNC__
 #define __FUNC__ __func__
#endif

inline constexpr bool is_dcheck =
#ifdef DCHECK
    true;
#else
    false;
#endif // DCHECK

inline constexpr bool is_debug =
#if defined(DEBUG) || defined(_DEBUG)
    true;
#else
    false;
#endif // defined(DEBUG) || defined(_DEBUG)

#endif // OSINFO_STDAFX_H_
