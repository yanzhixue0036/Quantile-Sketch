# Makefile for building and running the KLL sketch test

.PHONY: all build run clean

all: clean build run

build:
	meson setup build
	cd build && ninja

run:
	cd build && ./test_kll --filter=Uniform_randomized.delete

clean:
	rm -rf build
