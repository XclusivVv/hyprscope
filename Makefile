# Hyprland plugin makefile

PLUGIN_NAME = hyprscope

SOURCE_FILES = hyprscope.cpp

CXX = g++
CXXFLAGS = -std=c++23 -fPIC -DWLR_USE_UNSTABLE -I/usr/include/pixman-1 -I/usr/include/libdrm
LDFLAGS = -shared

all: check $(PLUGIN_NAME).so

check:
	@pkg-config --exists hyprland || (echo "Hyprland headers not found. Install hyprland development files." && exit 1)

$(PLUGIN_NAME).so: $(SOURCE_FILES)
	$(CXX) $(CXXFLAGS) $(SOURCE_FILES) $(LDFLAGS) -o $(PLUGIN_NAME).so `pkg-config --cflags hyprland pixman-1` `pkg-config --libs hyprland`

clean:
	rm -f $(PLUGIN_NAME).so

install: $(PLUGIN_NAME).so
	@echo "Use 'hyprpm add' to install this plugin"

.PHONY: all clean check install
