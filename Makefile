
.PHONY: default
default: build-release

build:
	if not exist build mkdir build
	echo "Generating CMAKE Build"
	cd build && cmake ../ -G "Ninja Multi-Config"

.PHONY: build-release
build-release: build
	cmake --build ./build --target SimulationGame --config Release

.PHONY: clean
clean:
	rmdir .\build /S /Q

.PHONY:
run:
	./build/Release/SimulationGame.exe