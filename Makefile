# osinfo.dll MAKEFILE for compiling under MinGW32

# Compiler and tools
CC       := i686-w64-mingw32-gcc
CXX      := i686-w64-mingw32-g++
LD       := i686-w64-mingw32-g++
DLLTOOL  := i686-w64-mingw32-dlltool
GENDEF   := gendef
RC       := i686-w64-mingw32-windres

# Targets
NAME     := osinfo
TARGET   := osinfo.dll

# Build type: choose Debug or Release
# Default is Release
BUILDTYPE ?= Debug

# Directories
DLL_DIR  := ./

# Automatically discover source files
# Dll source files
DLL_HEADERS := $(wildcard $(DLL_DIR)/*.h)
DLL_C       := $(wildcard $(DLL_DIR)/*.c)
DLL_CPP     := $(wildcard $(DLL_DIR)/*.cc)
DLL_RC      := $(wildcard $(DLL_DIR)/*.rc)

# Objects
DLL_OBJ_C    := $(DLL_C:.c=.o)
DLL_OBJ_CPP  := $(DLL_CPP:.cc=.o)
DLL_OBJ_RC   := $(DLL_RC:.rc=.res)

# Compiler flags #
DEFINES  := -DUNICODE -D_UNICODE -D_WINDOWS -DWINVER=0x0500 -D_WIN32_WINNT=0x0500 -D_WIN64_WINNT=0x0502 -D_WIN32_IE=0x501 -DOSINFO_DLL_EXPORTS=1
# Project-specific include dirs
#INCLUDE_DIRS = -Iosinfo
# Show all errors, compile everything static, ensure src dir is included, -municode
# since this is a GUI windows app, ensure relocatable data
CFLAGS   := $(DEFINES) -Wall -static-libgcc -municode -finput-charset=UTF-8 -fexec-charset=UTF-8 $(INCLUDE_DIRS)

# Compiler optimization and architecture flags
ifeq ($(BUILDTYPE), Release)
CFLAGS   += -O2 -g0 -s -MMD -MP -mfpmath=sse -mfxsr -msse -msse2 -DNDEBUG -D_NDEBUG -Wno-unused-function
endif
ifeq ($(BUILDTYPE), Debug)
CFLAGS   += -Og -g -MMD -MP -mfpmath=sse -mfxsr -msse -msse2 -DDEBUG -D_DEBUG
endif

# C++ only flags
CXXFLAGS := $(CFLAGS) -std=c++17 -static-libstdc++

# Libraries
LIB_DIR  := ./
LIBS     := -lkernel32
# Linker flags
LDFLAGS  := -L$(LIB_DIR) -municode -Wl,--subsystem,windows:5.00

# Include generated dependency files
-include $(DLL_OBJ_C:.o=.d)
-include $(DLL_OBJ_CPP:.o=.d)
-include $(DLL_OBJ_RC:.res=.d)

# Build Commands #
all: $(NAME)

$(NAME): osinfo.def $(TARGET)

# osinfo.dll
$(TARGET): $(DLL_OBJ_C) $(DLL_OBJ_CPP) $(DLL_OBJ_RC)
	$(LD) -static -shared $(DLL_OBJ_C) $(DLL_OBJ_CPP) $(DLL_OBJ_RC) $(LIBS) -o osinfo.dll

osinfo.def: osinfo.dll
	$(GENDEF) - osinfo.dll > $(DLL_DIR)/osinfo.def
	$(DLLTOOL) -k --output-lib $(DLL_DIR)/libosinfo.a --def $(DLL_DIR)/osinfo.def

# Compilation Rules #
# Compile C sources
%.o: %.h %.c 
	$(CC) $(CFLAGS) -static -c $< -o $@

# Compile C++ sources
%.o: %.h %.hpp %.cc %.cpp
	$(CXX) $(CXXFLAGS) -static -c $< -o $@

# Compile .rc → .o or .res
%.res: %.rc
	$(RC) $< -O coff $@

# Cleaning Rules #
clean:
	rm -f -v $(DLL_OBJ_C) $(DLL_OBJ_CPP) $(DLL_OBJ_RC) $(DLL_OBJ_C:.o=.d) $(DLL_OBJ_CPP:.o=.d) $(TARGET) osinfo.def

# Testing rules #
test:
	cat $(TARGET)

# PHONY targets for build deps tracking
.PHONY: all $(NAME) osinfo.def clean test
