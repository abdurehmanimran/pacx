codeFiles := src/pacx.c src/packageinfo.c src/packagelist.c src/downloader.c src/urls.c

pacx: src/pacx.c src/downloader.c src/urls.c
	mkdir -p bin
	cc $(codeFiles) -o bin/pacx -fsanitize=address -g -pthread

build-release: src/pacx.c src/downloader.c src/urls.c
	mkdir bin/release -p
	cc $(codeFiles) -o bin/release/pacx -O3
