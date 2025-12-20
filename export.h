#ifndef OSINFO_DLL_EXPORT_H_
#define OSINFO_DLL_EXPORT_H_

#ifndef DLL_IMPORT
 #define DLL_IMPORT __declspec(dllimport)
#endif

#ifndef DLL_EXPORT
 #define DLL_EXPORT __declspec(dllexport)
#endif

#endif // OSINFO_DLL_EXPORT_H_
