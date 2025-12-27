# Beginner-friendly Makefile for a single-file Raylib C++ program (macOS/Homebrew)
#
# Usage:
#   make        -> builds ./test_raylib
#   make run    -> builds (if needed) then runs
#   make clean  -> removes built files
#


APP      := test_raylib
SRC      := test_raylib.cpp
CXX      := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Prefer pkg-config if present (prints the correct include/lib flags automatically)
PKGCONFIG := $(shell command -v pkg-config 2>/dev/null)

# Homebrew raylib prefix (e.g. /opt/homebrew/opt/raylib)
RAYLIB_PREFIX := $(shell brew --prefix raylib 2>/dev/null)

# macOS frameworks raylib needs
MACOS_FRAMEWORKS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenAL

# Build flags depending on whether pkg-config is available
ifeq ($(PKGCONFIG),)
  # No pkg-config found: use Homebrew prefix
  CPPFLAGS := -I$(RAYLIB_PREFIX)/include
  LDFLAGS  := -L$(RAYLIB_PREFIX)/lib
  LDLIBS   := -lraylib $(MACOS_FRAMEWORKS)
else
  # pkg-config found: let it supply everything
  CPPFLAGS := $(shell pkg-config --cflags raylib)
  LDLIBS   := $(shell pkg-config --libs raylib)
endif

.PHONY: all run clean info

all: $(APP)

# Link step
$(APP): $(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)

info:
	@echo "APP             : $(APP)"
	@echo "SRC             : $(SRC)"
	@echo "CXX             : $(CXX)"
	@echo "PKGCONFIG        : $(PKGCONFIG)"
	@echo "RAYLIB_PREFIX    : $(RAYLIB_PREFIX)"
	@echo "CPPFLAGS         : $(CPPFLAGS)"
	@echo "LDFLAGS          : $(LDFLAGS)"
	@echo "LDLIBS           : $(LDLIBS)"
