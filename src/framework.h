#ifndef OSINFO_FRAMEWORK_H_
#define OSINFO_FRAMEWORK_H_

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
#include <cstdint>   // Other number types
#include <iomanip>   // stream modifiers
#include <iostream>  // std::cout std::wcout
#include <sstream>   // std::ostringstream std::wostringstream
#include <stdexcept> // exception handline
#include <string>    // std::string std::wstring
#include <vector>    // vector storage

/* Defines handling */

#ifndef __cplusplus
 #error This library requires a C++ compiler
#endif // !__cplusplus

#if __cplusplus < 201103L
 // For old compilers without constexpr or inline
 #if !defined(constexpr) || !defined(__cpp_constexpr)
  #define constexpr const
 #endif // constexpr
 #define inline
#endif  // __cplusplus < 201103L

// Alias
#ifndef __FUNC__
 #define __FUNC__ __func__
#endif

// Convert compiler defines to usable bools
inline constexpr bool is_dcheck =
#ifdef DCHECK_ON
    true;
#else
    false;
#endif // DCHECK_ON

inline constexpr bool is_debug =
#if defined(DEBUG) || defined(_DEBUG)
    true;
#else
    false;
#endif // DEBUG || _DEBUG

// Universal pre-processer check for x86_64 arch is nonexistant. Make our own.
inline constexpr bool is_x64 =
#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
    true;
 #define IS_X64
#else
 #define IS_X86
    false;
#endif // defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)

// Sanity check of above
#if defined(IS_X86) && defined(IS_X64)
 #error IS_X86 and IS_X64 both defined!
#endif

#endif // OSINFO_FRAMEWORK_H_
