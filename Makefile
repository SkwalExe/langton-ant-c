all: build

build:
	mkdir -p build
	clang src/*.c -o build/langton

install:
	[ -f build/langton ] || (echo Please build first; exit 1)
	sudo cp build/langton /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/langton