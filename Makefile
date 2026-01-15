pacx: src/pacx.c src/downloader.c src/urls.c
	mkdir bin
	cc src/pacx.c src/downloader.c src/urls.c -o bin/pacx
