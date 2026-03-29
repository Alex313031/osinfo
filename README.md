# osinfo.dll

 - A small DLL to get useful Windows OS version info.

It is compatible with Windows NT 4.0 all the way to Windows 11.

The primary consumer of this project is [CryoCalc](https://github.com/Alex313031/cryocalc), which uses it to 
decide which functions to use depending on the reported Windows version.

Note that this DLL uses the [`RtlGetVersion()`](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlgetversion) function,
which can be spoofed with the "compatibility" tab of the Properties dialog on an .exe.

I may use the undocumented [`RtlGetNtVersionNumbers()`](https://www.geoffchappell.com/studies/windows/win32/ntdll/api/ldrinit/getntversionnumbers.htm)
function in the future, since it directly fetches the real NT version numbers from inside `ntdll.dll`.

## Usage

Include `os_info_dll.h` in your .exe's source C/C++ file(s) where you need it.

This file lists all the functions the DLL provides, as well as NT version constants for the version comparison functions,
and typedefs for dynamically accessing the functions therein using [`GetProcAddress()`](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress).

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
 Uses the standard Microsoft [DllGetVersion function prototype](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/bb776779(v=vs.85)#using-dllgetversion-to-determine-the-version-number).
