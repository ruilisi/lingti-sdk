# Lingti SDK Makefile
# Usage: make example
# Note: The SDK is designed for Windows. This Makefile works on both Windows and Unix-like systems.

# Detect OS
ifeq ($(OS),Windows_NT)
    # Real Windows kernel, but we need to detect whether Make is running under
    # CMD/PowerShell or MSYS/MinGW/Cygwin (Git Bash).
    ifeq ($(shell uname 2>/dev/null),)
        # CMD / PowerShell (no uname output)
        EXE_EXT = .exe
        RM = del /Q
        RMDIR = rmdir /S /Q
        MKDIR = mkdir
        CP = copy
        SEP = \\
    else
        # Git Bash / MSYS / Cygwin running on Windows
        EXE_EXT = .exe
        RM = rm -f
        RMDIR = rm -rf
        MKDIR = mkdir -p
        CP = cp
        SEP = /
    endif
else
    # Pure Linux / macOS
    EXE_EXT = 
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    CP = cp
    SEP = /

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

# Shared flags
SDK_LIB = lingti_sdk.lib
LDFLAGS = $(SDK_LIB)

# GitHub release settings
GITHUB_REPO = ruilisi/lingti-sdk
SDK_VERSION = $(shell grep '^ \* Version:' lingti_sdk.h | sed 's/.*Version: //')
DOWNLOAD_URL_LIB = https://github.com/$(GITHUB_REPO)/releases/download/v$(SDK_VERSION)/$(SDK_LIB)
DOWNLOAD_URL_DLL = https://github.com/$(GITHUB_REPO)/releases/download/v$(SDK_VERSION)/$(SDK_DLL)

# Directories and files
OUTPUT_DIR = dist
EXAMPLE_SRC = sdk_example.c
EXAMPLE_BIN = example$(EXE_EXT)
DRIVER_FILE = lingtiwfp64.sys
SDK_DLL = lingti_sdk.dll

# Targets
.PHONY: all example clean help check-lib delete-latest-release

all: example

check-lib:
	@echo "Downloading latest $(SDK_LIB) version $(SDK_VERSION)..."
	@echo "Download URL: $(DOWNLOAD_URL_LIB)"
	@curl -L -o $(SDK_LIB) $(DOWNLOAD_URL_LIB) || \
		(echo "Failed to download $(SDK_LIB). Please download manually from $(DOWNLOAD_URL_LIB)" && exit 1)
	@echo "Successfully downloaded $(SDK_LIB)"
	@echo "Downloading latest $(SDK_DLL) version $(SDK_VERSION)..."
	@echo "Download URL: $(DOWNLOAD_URL_DLL)"
	@curl -L -o $(SDK_DLL) $(DOWNLOAD_URL_DLL) || \
		(echo "Failed to download $(SDK_DLL). Please download manually from $(DOWNLOAD_URL_DLL)" && exit 1)
	@echo "Successfully downloaded $(SDK_DLL)"

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
ifeq ($(OS),Windows_NT)
	@echo "clean wfp driver"
	-sc.exe stop lingtiwfp
	-sc.exe delete lingtiwfp
endif
	$(RMDIR) $(OUTPUT_DIR)

delete-latest-release:
	@echo "Fetching latest release..."
	@LATEST_TAG=$$(gh release list --limit 1 --json tagName --jq '.[0].tagName'); \
	if [ -z "$$LATEST_TAG" ]; then \
		echo "No releases found."; \
		exit 1; \
	fi; \
	echo "Deleting release $$LATEST_TAG..."; \
	gh release delete "$$LATEST_TAG" --yes && \
	echo "Deleting tag $$LATEST_TAG..." && \
	git push --delete origin "$$LATEST_TAG" && \
	git tag -d "$$LATEST_TAG" && \
	echo "Successfully deleted release and tag $$LATEST_TAG"

help:
	@echo "Lingti SDK Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make example               - Build example and copy files to example/ directory"
	@echo "  make clean                 - Remove the example/ directory"
	@echo "  make delete-latest-release - Delete the latest GitHub release and tag"
	@echo "  make help                  - Show this help message"
