#!/bin/bash

# make.sh for building srne.ino Arduino project

# Arduino CLI path
ARDUINO_CLI="/Users/kevin/bin/arduino-cli"

# Board FQBN from build.options.json
FQBN="esp32:esp32:esp32:JTAGAdapter=default,PSRAM=disabled,PartitionScheme=huge_app,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none,EraseFlash=none,ZigbeeMode=default"

# Serial port
PORT="/dev/tty.usbserial-0001"

# Sketch file
SKETCH="srne.ino"

# Build directory
BUILD_DIR="build"

# Function to build
build() {
    clear
    # rm -rf $BUILD_DIR/*
    $ARDUINO_CLI compile --fqbn $FQBN --build-path $BUILD_DIR $SKETCH
}

# Function to clean
clean() {
    rm -rf $BUILD_DIR/*
}

# Function to upload
upload() {
    clear
    $ARDUINO_CLI upload --fqbn $FQBN --port $PORT --build-path $BUILD_DIR $SKETCH
    $ARDUINO_CLI monitor --port $PORT --config 115200
}

# Function to monitor
monitor() {
    $ARDUINO_CLI monitor --port $PORT --config 115200
}

# Main logic
case "$1" in
    all|build)
        build
        ;;
    clean)
        clean
        ;;
    upload|install)
        upload
        ;;
    monitor)
        monitor
        ;;
    run)
        build
        upload
        ;;
    *)
        echo "Usage: $0 {all|build|clean|upload|install|monitor|run}"
        exit 1
        ;;
esac
