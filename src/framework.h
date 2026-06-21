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
 #define IS_DCHECK 1
#else
    false;
#endif // DCHECK_ON

inline constexpr bool is_debug =
#if defined(DEBUG) || defined(_DEBUG)
    true;
 #define IS_DEBUG 1
#else
    false;
#endif // DEBUG || _DEBUG

// Universal pre-processer check for x86_64 arch is nonexistant. Make our own.
inline constexpr bool is_x64 =
#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
    true;
 #define IS_X64 1
#else
 #define IS_X86 1
    false;
#endif // defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)

// Sanity checks of above
#if defined(IS_X86) && defined(IS_X64)
 #error IS_X86 and IS_X64 both defined!
#endif
#if !defined(IS_X86) && !defined(IS_X64)
 #error IS_X86 or IS_X64 must be defined
#endif

// Build-time diagnostics. #pragma message takes a string literal, so numeric
// macros must be run through STRINGIZE() (from version.h) and joined with
// adjacent-literal concatenation; the parenthesized form works on MSVC/GCC/Clang.
#ifdef IS_DCHECK

 #if defined(IS_X64)
  #pragma message("Arch: x64")
 #else
  #pragma message("Arch: x86")
 #endif

 #if defined(_MSC_VER)
  #pragma message("_MSC_VER " STRINGIZE(_MSC_VER))
 #endif // _MSC_VER
 // Clang defines __GNUC__ too, so report it as GCC only when it isn't Clang.
 #if defined(__GNUC__) && !defined(__clang__)
  #pragma message("GCC version " STRINGIZE(__GNUC__) "." STRINGIZE(__GNUC_MINOR__) "." STRINGIZE(__GNUC_PATCHLEVEL__))
 #endif // __GNUC__ && !__clang__
 #if defined(__clang__)
  #pragma message("Clang version " STRINGIZE(__clang_major__) "." STRINGIZE(__clang_minor__) "." STRINGIZE(__clang_patchlevel__))
 #endif // __clang__
 #if defined(__MINGW32__)
  #pragma message( \
      "__MINGW32__ " STRINGIZE(__MINGW32_MAJOR_VERSION) "." STRINGIZE(__MINGW32_MINOR_VERSION))
 #endif // __MINGW32__
 #if defined(__MINGW64__)
  #pragma message("__MINGW64__ " STRINGIZE(__MINGW64_VERSION_MAJOR) "." STRINGIZE(__MINGW64_VERSION_MINOR) "." STRINGIZE(__MINGW64_VERSION_BUGFIX))
 #endif // __MINGW64__
 #if defined(__WATCOMC__)
  #pragma message("__WATCOMC__ " STRINGIZE(__WATCOMC__))
 #endif // __WATCOMC__
 #if defined(__INTEL_COMPILER)
  #pragma message("__ICC " STRINGIZE(__INTEL_COMPILER))
 #endif // __INTEL_COMPILER

#endif  // IS_DCHECK

#endif  // OSINFO_FRAMEWORK_H_
