SRC_DIR := src/
FILES := $(wildcard $(SRC_DIR)*.c)

WARNING_FLAGS := -Wall -Wextra
DEBUG_FLAGES:= -g -fsanitize=address

build: $(FILES)
	mkdir -p bin
	cc $(FILES) -o bin/pacx -pthread $(DEBUG_FLAGES) $(WARNING_FLAGS)

build-release: $(FILES)
	mkdir bin/release -p
	cc $(FILES) -o bin/release/pacx -O3

install:
	mkdir /usr/share/pacx/cache -p
	mkdir /usr/share/pacx/log -p
	cp bin/release/pacx /usr/bin/pacx

install-debug:
	mkdir /usr/share/pacx/cache -p
	mkdir /usr/share/pacx/log -p
	cp bin/pacx /usr/bin/pacx
