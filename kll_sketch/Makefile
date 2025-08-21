# Makefile for building and running the KLL sketch test

.PHONY: all build run clean

all: build run

build:
	meson setup build
	cd build && ninja

run:
	cd build && ./test_kll

clean:
	rm -rf build
