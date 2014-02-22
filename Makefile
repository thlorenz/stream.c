UV_PATH=$(shell pwd)/libuv
UV_LIB=$(UV_PATH)/out/Debug/libuv.a

CFLAGS=-g -Wall -I$(UV_PATH)/include/ -std=c99

$(UV_LIB):
	cd $(UV_PATH) && \
	test -d ./build/gyp || (mkdir -p ./build && git clone https://git.chromium.org/external/gyp.git ./build/gyp) && \
	./gyp_uv.py -f make && \
	$(MAKE) -C ./out

MAIN_SRC=main.c
EXECUTABLE=main

all: clean $(EXECUTABLE) 

run: all
	./main

$(EXECUTABLE): $(UV_LIB) $(MAIN_SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
	rm -f main main.o

.PHONY: all clean
