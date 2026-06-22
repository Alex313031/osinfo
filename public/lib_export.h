#ifndef OSINFO_LIB_EXPORT_H_
#define OSINFO_LIB_EXPORT_H_

// NOTE: Don't include this file in your project, include libosinfo.h instead.

// clang-format off

// Pick exactly ONE of these when building against osinfo:
//   STATIC_OSINFO  - building or consuming the static library (.lib/.a)
//   SHARED_OSINFO  - building or consuming the shared library (.dll)
//
// When building the osinfo DLL itself, the build additionally defines DLL_EXPORT
// so that symbols are exported; consumers leave it undefined and therefore import.
// A static library has no import/export, so it needs nothing beyond STATIC_OSINFO.
//
// OSINFO_API annotates every public function and expands to the correct
// __declspec (or to nothing for the static lib) based on the above.
#if defined(STATIC_OSINFO) && defined(SHARED_OSINFO)
 #error "osinfo: define only one of STATIC_OSINFO or SHARED_OSINFO"
#elif defined(STATIC_OSINFO)
 #define OSINFO_API
#elif defined(SHARED_OSINFO)
 #if defined(DLL_EXPORT)
  #define OSINFO_API __declspec(dllexport)
 #else
  #define OSINFO_API __declspec(dllimport)
 #endif
#else
 #error "osinfo: define STATIC_OSINFO (static lib) or SHARED_OSINFO (DLL) before including libosinfo.h"
#endif

// clang-format on

#endif // OSINFO_LIB_EXPORT_H_
