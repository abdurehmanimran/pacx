pacx: src/pacx.c src/downloader.c src/urls.c
	mkdir -p bin
	cc src/pacx.c src/downloader.c src/urls.c -o bin/pacx -fsanitize=address
