#!/bin/bash

SRC="main.c"
SRC_BUGTEST="bugtest.c"

OUT_DIR="build"
OUT="$OUT_DIR/enniolimpiadi"
OUT_BUGTEST="$OUT_DIR/bugtest"

# -Werror?
CFLAGS="-fcolor-diagnostics -fansi-escape-codes -fsanitize=address -std=c99 -Wall -Wconversion -pedantic -g -O0"
PKG_FLAGS=$(pkg-config --cflags --libs raylib)

# Show help message
show_help() {
    echo "Usage: $0 [main|bugtest|clean]"
    echo
    echo "  main       Compile main.c -> $OUT"
    echo "  bugtest    Compile bugtest.c -> $OUT_BUGTEST"
    echo "  testnames  Compile tests/test_names.c -> tests/build/test_names"
    echo "  clean      Remove the build directory"
    echo
}

# No argument passed → show help
if [[ $# -eq 0 ]]; then
    echo "❌ No command provided."
    show_help
    exit 1
fi

# Handle 'clean' command first
if [[ "$1" == "clean" ]]; then
    echo "Cleaning build directory..."
    rm -rf "$OUT_DIR"
    echo "🧹 Cleaned."
    exit 0
fi

# Ensure build directory exists
if [ ! -d "$OUT_DIR" ]; then
    echo "Creating build directory..."
    mkdir "$OUT_DIR"
fi

# Choose what to compile
case "$1" in
    main)
        echo "Compiling $SRC -> $OUT"
        clang $CFLAGS $SRC -o $OUT $PKG_FLAGS
        ;;
    bugtest)
        echo "Compiling $SRC_BUGTEST -> $OUT_BUGTEST"
        clang $CFLAGS $SRC_BUGTEST -o $OUT_BUGTEST $PKG_FLAGS
        ;;
    testnames)
        echo "Compiling tests/test_names.c -> $OUT_DIR/test_names"
        clang $CFLAGS ./tests/test_names.c -o $OUT_DIR/test_names
        ;;
    *)
        echo "❌ Unknown command: $1"
        show_help
        exit 1
        ;;
esac

# Check result of the last command (compilation)
if [ $? -eq 0 ]; then
    echo "✅ Build successful."
else
    echo "❌ Build failed."
    exit 1
fi
