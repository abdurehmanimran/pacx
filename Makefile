files := src/pacx.c src/packageinfo.c src/packagelist.c src/downloader.c src/urls.c

pacx: src/pacx.c src/downloader.c src/urls.c
	mkdir -p bin
	cc $(files) -o bin/pacx -fsanitize=address

build-release: src/pacx.c src/downloader.c src/urls.c
	mkdir bin/release -p
	cc $(files) -o bin/release/pacx -O3
