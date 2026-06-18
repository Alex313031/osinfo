#ifndef OSINFO_LIB_EXPORT_H_
#define OSINFO_LIB_EXPORT_H_

// NOTE: Don't include this file in your project, include libosinfo.h instead.

// Tiny helper for making code that can be used for both .dlls and static .libs.
#ifndef DLL_IMPORT
 #define DLL_IMPORT __declspec(dllimport)
#endif

#ifndef DLL_EXPORT
 #define DLL_EXPORT __declspec(dllexport)
#endif

// Use OSINFO_API for exported functions. When building the DLL, define
// OSINFO_DLL_EXPORTS to export symbols.
#if defined(OSINFO_DLL_EXPORTS)   // For use by osinfo when building DLL
 #define OSINFO_API DLL_EXPORT
#elif defined(OSINFO_DLL_IMPORTS) // Only for use by consumers (of either static lib or .dll).
 #define OSINFO_API DLL_IMPORT
#elif defined(OSINFO_STATIC_LIB)  // For use by osinfo when building static library.
 #define OSINFO_API
#endif                            // OSINFO_DLL_EXPORTS

#endif // OSINFO_LIB_EXPORT_H_
