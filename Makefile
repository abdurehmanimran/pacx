FILES := src/pacx.c src/packageinfo.c src/packagelist.c src/downloader.c src/urls.c src/print.c src/progress.c

WARNING_FLAGS := -Wall -Wextra
DEBUG_FLAGES:= -g -fsanitize=address

pacx: src/pacx.c src/downloader.c src/urls.c
	mkdir -p bin
	cc $(FILES) -o bin/pacx -pthread $(DEBUG_FLAGES) $(WARNING_FLAGS)

build-release: src/pacx.c src/downloader.c src/urls.c
	mkdir bin/release -p
	cc $(FILES) -o bin/release/pacx -O3

install:
	mkdir -p /usr/local/share/pacx/cache
	cp bin/release/pacx /usr/local/bin/pacx

uninstall:
	rm -f /usr/local/bin/pacx
	rm -rf /usr/local/share/pacx