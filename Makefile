CMAKE ?= cmake
BUILD_DIR ?= build
MT_BUILD_DIR ?= build-mt
BUILD_TYPE ?= Release

.DEFAULT_GOAL := build

.PHONY: configure build test mt-configure mt-build mt-test docker-test tsan-configure tsan-build tsan-test

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

mt-configure:
	$(CMAKE) -S . -B $(MT_BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DTERMINAL_TACTICS_BUILD_MT=ON

mt-build: mt-configure
	$(CMAKE) --build $(MT_BUILD_DIR) --parallel

mt-test: mt-build
	ctest --test-dir $(MT_BUILD_DIR) --output-on-failure

docker-test:
	docker compose run --build --rm tests

tsan-configure:
	$(CMAKE) --preset thread-sanitizer

tsan-build: tsan-configure
	$(CMAKE) --build --preset thread-sanitizer

tsan-test: tsan-build
	ctest --preset thread-sanitizer
