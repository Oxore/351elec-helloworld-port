CC=$(TOOLCHAIN_DIR)/bin/aarch64-libreelec-linux-gnueabi-gcc
CXX=$(TOOLCHAIN_DIR)/bin/aarch64-libreelec-linux-gnueabi-g++

main: main.o

clean:
	rm -rfv *.o main
