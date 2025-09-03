# Makefile for building srne.ino Arduino project

# Arduino CLI path
ARDUINO_CLI = /Users/kevin/bin/arduino-cli

# Board FQBN from build.options.json
FQBN = esp32:esp32:esp32:JTAGAdapter=default,PSRAM=disabled,PartitionScheme=huge_app,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none,EraseFlash=none,ZigbeeMode=default

# Serial port
PORT = /dev/tty.usbserial-0001

# Sketch file
SKETCH = srne.ino

# Build directory
BUILD_DIR = build

# Default target
all: build

run:
	clear
	$(MAKE) build
	$(MAKE) upload

# Build the project
build:
# 	clear
# 	rm -rf $(BUILD_DIR)/*
	$(ARDUINO_CLI) compile --fqbn $(FQBN) --build-path $(BUILD_DIR) $(SKETCH)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)/*

# Upload to board (using PORT variable, adjust as needed)
install:upload

upload:
	$(ARDUINO_CLI) upload --fqbn $(FQBN) --port $(PORT) --build-path $(BUILD_DIR) $(SKETCH)
	$(ARDUINO_CLI) monitor --port $(PORT) --config 115200

# Monitor serial output
monitor:
	$(ARDUINO_CLI) monitor --port $(PORT) --config 115200

.PHONY: all build clean upload monitor
