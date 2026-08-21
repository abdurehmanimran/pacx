NAME := pacx
SRC_DIR := src
FILES := $(wildcard $(SRC_DIR)/*.c)

WARNING_FLAGS := -Wall -Wextra
DEBUG_FLAGES:= -g -fsanitize=address

BUILD_DIR:= bin
BINARY := ${BUILD_DIR}/${NAME}
REL_BINARY := ${BUILD_DIR}/release/${NAME}

build ${BINARY}: ${FILES}
	mkdir -p bin
	cc ${FILES} -o ${BINARY} -pthread ${DEBUG_FLAGES} ${WARNING_FLAGS}

build-release ${REL_BINARY}: ${FILES}
	mkdir bin/release -p
	cc ${FILES} -o ${REL_BINARY} -O3

install: ${REL_BINARY}
	mkdir /usr/share/pacx/cache -p
	mkdir /usr/share/pacx/log -p
	cp ${REL_BINARY} /usr/bin/pacx

install-debug: ${BINARY}
	mkdir /usr/share/pacx/cache -p
	mkdir /usr/share/pacx/log -p
	cp ${BINARY} /usr/bin/pacx
