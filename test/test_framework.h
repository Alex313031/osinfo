#ifndef TEST_OSINFO_TEST_FRAMEWORK_H_
#define TEST_OSINFO_TEST_FRAMEWORK_H_

// Keep this at top
#include "version.h"

// Test apps are console programs, we don't need all of windows.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // Main Windows header
#include <tchar.h>   // Auto deduce TCHAR and handle _UNICODE

// STL includes
#include <iomanip>  // steam manipulation
#include <iostream> // so we can output to the console
#include <string>   // std::string/std::wstring

#endif // TEST_OSINFO_TEST_FRAMEWORK_H_
