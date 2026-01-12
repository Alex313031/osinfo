#ifndef OSINFO_DLL_EXPORT_H_
#define OSINFO_DLL_EXPORT_H_

#ifndef DLL_IMPORT
 #define DLL_IMPORT __declspec(dllimport)
#endif

#ifndef DLL_EXPORT
 #define DLL_EXPORT __declspec(dllexport)
#endif

// Use OSINFO_API for exported functions. When building the DLL, define
// OSINFO_DLL_EXPORTS to export symbols. When importing, they'll be imported.
#ifdef OSINFO_DLL_EXPORTS
 #define OSINFO_API DLL_EXPORT
#else
 #define OSINFO_API DLL_IMPORT
#endif

#endif // OSINFO_DLL_EXPORT_H_
