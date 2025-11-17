
.PHONY: default
default: build-release

build:
	if not exist build mkdir build

build/build.ninja: build
	echo "Generating CMAKE Build"
	cd build && cmake ../ -G "Ninja Multi-Config"

.PHONY: build-release
build-release: build/build.ninja
	cmake --build ./build --target SimulationGame --config Release

.PHONY: build-debug
build-debug: build/build.ninja
	cmake --build ./build --target SimulationGame --config Debug

.PHONY: clean
clean:
	rmdir .\build /S /Q

.PHONY:
run:
	./build/Release/SimulationGame.exe gamePath=$(CURDIR)

.PHONY:
shell:
	VsDevCmd.bat

.PHONY:
list-targets: build/build.ninja
	cmake --build ./build --target help