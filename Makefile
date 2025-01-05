######################################################################
# @author      : stanleyarn (stanleyarn@$HOSTNAME)
# @file        : Makefile
# @created     : Lundi déc 23, 2024 16:24:14 CET
######################################################################

######################################################################
#### Variables and settings
######################################################################

# Executable name
EXEC = NavigoMaze

# Build directory
BUILD_DIR_ROOT = build

# Sources
SRC_DIR = src
SRCS := $(sort $(shell find $(SRC_DIR) -name '*.c'))

# Library directory
LIBS_DIR = libs

# Includes
INCLUDE_DIR = $(LIBS_DIR)/glew/include \
			  $(LIBS_DIR)/glfw/include \
			  $(LIBS_DIR)/cglm/include \
			  $(LIBS_DIR)/csimple-ecs/include \
			  $(LIBS_DIR)/uthash/include \
			  $(LIBS_DIR)/stb \
			  $(LIBS_DIR)/jsmn
INCLUDES = $(addprefix -I,$(SRC_DIR) $(INCLUDE_DIR))

# C preprocessor settings
CPPFLAGS = $(INCLUDES) -MMD -MP

# C compiler settings
CC = gcc
CFLAGS = -g -Wall -std=gnu99

# Linker flags
LDFLAGS =
LDLIBS =

# Target OS detection
ifeq ($(OS),Windows_NT)
	OS = windows
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		OS = macos
	else ifeq ($(UNAME),Linux)
		OS = linux
	else
    	$(error OS not supported by this Makefile)
	endif
endif

# Platform-specific settings
ifeq ($(OS),windows)
	# Link libgcc and libstdc++ statically on Windows
	LDFLAGS += -static-libgcc -static-libstdc

	# Windows-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
else ifeq ($(OS),macos)
	# macOS-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS += -framework OpenGL -framework Cocoa -framework IOKit \
			  $(LIBS_DIR)/glew/lib/libGLEW.a \
			  $(LIBS_DIR)/glfw/lib-universal/libglfw3.a \
			  $(LIBS_DIR)/csimple-ecs/lib/csimple_ecs.a
else ifeq ($(OS),linux)
	# Linux-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS += -ldl -lm -lpthread -lGL -lGLU -lX11 -lGLEW -lglfw
endif

######################################################################
#### Final setup
######################################################################

# Add .exe extension to executables on Windows
ifeq ($(OS),windows)
	EXEC := $(EXEC).exe
endif

BUILD_DIR := $(BUILD_DIR_ROOT)/$(OS)
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# Object files
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Dependency files
DEPS := $(OBJS:.o=.d)

# Compilation database fragments
COMPDBS := $(OBJS:.o=.json)

# All files (sources and headers) (for formatting and linting)
FILES := $(shell find $(SRC_DIR) $(INCLUDE_DIR) -name '*.c' -o -name '*.h' -o -name '*.inl' 2>/dev/null)

######################################################################
#### Targets
######################################################################

# Disable default implicit rules
.SUFFIXES:

.PHONY: all
all: $(BIN_DIR)/$(EXEC)

# Build executable
$(BIN_DIR)/$(EXEC): $(OBJS)
	@echo "Detected platform : $(OS)"
	@echo "Building executable: $@"
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C source files
$(OBJS): $(OBJ_DIR)/%.o: %.c
	@echo "Compiling: $<"
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Include automatically-generated dependency files
-include $(DEPS)

.PHONY: run
run: $(BIN_DIR)/$(EXEC)
	# @cd $(BIN_DIR) && ./$(EXEC)
	$(BIN_DIR)/$(EXEC)

.PHONY: clean
clean:
	$(RM) -rf $(BUILD_DIR)

.PHONY: compdb
compdb: $(BUILD_DIR_ROOT)/compile_commands.json

# Generate JSON compilation database (compile_commands.json) by merging fragments
$(BUILD_DIR_ROOT)/compile_commands.json: $(COMPDBS)
	@echo "Generating: $@"
	@mkdir -p $(@D)
	@printf "[\n" > $@
	@for file in $(COMPDBS); do sed -e '$$s/$$/,/' "$${file}"; done | sed -e '$$s/,$$//' -e 's/^/    /' >> $@
	@printf "]\n" >> $@

# Generate JSON compilation database fragments from source files
$(COMPDBS): $(OBJ_DIR)/%.json: %.c
	@mkdir -p $(@D)
	@printf "\
	{\n\
	    \"directory\": \"$(CURDIR)\",\n\
	    \"command\": \"$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $(basename $@).o\",\n\
	    \"file\": \"$<\"\n\
	}\n" > $@

.PHONY: help
help:
	@echo "\
	Usage: make target... [options]...\n\
	\n\
	Targets:\n\
	  all             Build executable (default target)\n\
	  run             Build and run executable\n\
	  clean           Clean build directory (all platforms)\n\
	  compdb          Generate JSON compilation database (compile_commands.json)\n\
	  help            Print this information\n"
