ifneq ($(TOOLCHAIN_DIR),)
	CC=$(TOOLCHAIN_DIR)/bin/aarch64-libreelec-linux-gnueabi-gcc
	CXX=$(TOOLCHAIN_DIR)/bin/aarch64-libreelec-linux-gnueabi-g++
	SDL2_CONFIG=$(TOOLCHAIN_DIR)/aarch64-libreelec-linux-gnueabi/sysroot/usr/bin/sdl2-config
else
	SDL2_CONFIG=sdl2-config
endif

COMMON_COMPILE_FLAGS=$(shell $(SDL2_CONFIG) --cflags) -Wall -Wextra -ggdb
CFLAGS=$(COMMON_COMPILE_FLAGS)
CXXFLAGS=$(COMMON_COMPILE_FLAGS)

LDFAGS=$(shell $(SDL2_CONFIG) --libs) -lSDL2_ttf -lSDL2_image

main: main.o
	$(CXX) $(LDFAGS) $^ -o $@

clean:
	rm -rfv *.o main
