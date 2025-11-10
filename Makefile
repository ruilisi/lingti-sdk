# Lingti SDK Makefile
# Usage: make example
# Note: The SDK is designed for Windows. This Makefile works on both Windows and Unix-like systems.

# Detect OS
ifeq ($(OS),Windows_NT)
	# Windows (native cmd/PowerShell)
	EXE_EXT = .exe
	RM = del /Q
	RMDIR = rmdir /S /Q
	MKDIR = mkdir
	CP = copy
	SEP = \\
	SDK_LIB = lingti_sdk.lib
	LDFLAGS = lingti_sdk.lib
else
	# Unix-like systems (Linux, macOS, MinGW/MSYS, Cygwin)
	EXE_EXT = .exe
	RM = rm -f
	RMDIR = rm -rf
	MKDIR = mkdir -p
	CP = cp
	SEP = /
	SDK_LIB = lingti_sdk.lib
	# Use .lib file for cross-compilation or MinGW
	LDFLAGS = lingti_sdk.lib
endif

# Compiler settings
# Automatically detect and use the appropriate compiler
ifeq ($(OS),Windows_NT)
	CC = gcc
else
	# Check if MinGW cross-compiler is available
	MINGW_CC := $(shell which x86_64-w64-mingw32-gcc 2>/dev/null)
	ifneq ($(MINGW_CC),)
		CC = x86_64-w64-mingw32-gcc
	else
		CC = gcc
		$(warning MinGW cross-compiler not found. Install it with: brew install mingw-w64)
	endif
endif
CFLAGS = -Wall -O2 -I.

# Directories and files
OUTPUT_DIR = dist
EXAMPLE_SRC = sdk_example.c
EXAMPLE_BIN = example$(EXE_EXT)
DRIVER_FILE = lingtiwfp64.sys
SDK_DLL = lingti_sdk.dll

# Targets
.PHONY: all example clean help

all: example

example:
	@echo "Building example..."
	@$(MKDIR) $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(EXAMPLE_SRC) $(LDFLAGS) -o $(OUTPUT_DIR)/$(EXAMPLE_BIN)
ifeq ($(OS),Windows_NT)
	$(CP) $(DRIVER_FILE) $(OUTPUT_DIR)$(SEP)
	$(CP) $(SDK_DLL) $(OUTPUT_DIR)$(SEP)
else
	$(CP) $(DRIVER_FILE) $(OUTPUT_DIR)/
	$(CP) $(SDK_DLL) $(OUTPUT_DIR)/
endif
	@echo "Build complete! Files are in $(OUTPUT_DIR)/"
	@echo "  - $(EXAMPLE_BIN)"
	@echo "  - $(DRIVER_FILE)"
	@echo "  - $(SDK_DLL)"

clean:
	$(RMDIR) $(OUTPUT_DIR)

help:
	@echo "Lingti SDK Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make example  - Build example and copy files to example/ directory"
	@echo "  make clean    - Remove the example/ directory"
	@echo "  make help     - Show this help message"
