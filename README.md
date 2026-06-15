# OSInfo Library

 - A small DLL to get useful Windows OS version info, and CPUID info. It can also be used as a static library.

It is specifically designed to be legacy-compatible; it runs on Windows NT 4.0 all the way to Windows 11 26H2!  

Note that this DLL uses the [`RtlGetVersion()`](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlgetversion) function,
which can be spoofed with the "compatibility" tab of the Properties dialog on an .exe.

However, it also optionally can use the non-spoofable undocumented [`RtlGetNtVersionNumbers()`](https://www.geoffchappell.com/studies/windows/win32/ntdll/api/ldrinit/getntversionnumbers.htm) function.

## Usage

Include [`libosinfo.h`](./public/libosinfo.h) in your .exe's source C/C++ file(s) where you need it.

This header declares all the functions the DLL provides, as well as NT version constants for the version comparison functions,
and typedefs for dynamically accessing the functions using [`GetProcAddress()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress).  

Some useful functions include:  
`GetOSNameW()`: Returns the Windows version as a human readable string, like *"Windows 7 Professional Service Pack 1"*.

`GetShortNTVer()`: Returns the NT version as an unsigned long mirroring the values used in the windows headers.
 For example, Windows XP would be __0x0501__.

`GetServicePackNumber()`: Returns the service pack number as an int, 0 if RTM (no service pack).

`IsAtLeast(unsigned long)`: Checks if the Windows version is at least as high as the supplied version.
 For example, running IsAtLeast(0x0501UL) would check that the system is running at least Windows XP.

`IsWinOlderThan()`: Checks if the Windows version is older than the supplied value.

`GetWinVersionW()`: Returns the Windows NT version as a human readable string, for example *"5.0.2195"* (Windows 2000).

`DllGetVersion(DLLVERSIONINFO)`: Fetches the version of *osinfo.dll*.
 Uses the standard Microsoft [DllGetVersion function prototype](./docs/shlwapi.md#using-the-dllgetversion-function).

## Building

### Makefile
Currently, the makefile only supports 32 bit MinGW, and only the .dll. I will update it in the future to support static lib and x64 compilation.

### BUILD.gn
The main way this library is compiled and used is via [GN-Legacy](https://github.com/Alex313031/gn-legacy),
a [GN](https://gn.googlesource.com/gn/), [Ninja](https://ninja-build.org/), and [MinGW](https://www.mingw-w64.org/) toolchain for targeting legacy Windows.  
See the [README.md](https://github.com/Alex313031/gn-legacy#readme) there for usage.
