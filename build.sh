#!/bin/bash
set -x

SRC="main.c"
SRC_BUGTEST="bugtest.c"
SRC_BUGTEST2="bugtest2.c"

OUT_DIR="build"
OUT="$OUT_DIR/enniolimpiadi"
OUT_BUGTEST="$OUT_DIR/bugtest"
OUT_BUGTEST2="$OUT_DIR/bugtest2"

# -Werror?
CFLAGS="-fcolor-diagnostics -fansi-escape-codes -fsanitize=address -std=c99 -Wall -Wconversion -pedantic -g -O0"
PKG_FLAGS=$(pkg-config --cflags --libs raylib)

# Show help message
show_help() {
    echo "Usage: $0 [main|bugtest|clean]"
    echo
    echo "  enniolimpiadi    Compile main.c -> $OUT"
    echo "  bugtest          Compile bugtest.c  -> $OUT_BUGTEST"
    echo "  bugtest2         Compile bugtest2.c -> $OUT_BUGTEST2"
    echo "  testnames        Compile tests/test_names.c -> build/test_names"
    echo "  teststring       Compile tests/test_string.c -> build/test_string"
    echo "  testregistration Compile tests/test_registration.c -> build/test_registration"
    echo "  testarena        Compile tests/test_arena.c -> build/test_arena"
    echo "  clean            Remove the build directory"
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
    enniolimpiadi)
        echo "Compiling $SRC -> $OUT"
        clang $CFLAGS $SRC -o $OUT $PKG_FLAGS
        ;;
    bugtest)
        echo "Compiling $SRC_BUGTEST -> $OUT_BUGTEST"
        clang $CFLAGS $SRC_BUGTEST -o $OUT_BUGTEST $PKG_FLAGS
        ;;
    bugtest2)
        echo "Compiling $SRC_BUGTEST2 -> $OUT_BUGTEST2"
        clang $CFLAGS $SRC_BUGTEST2 -o $OUT_BUGTEST2
        ;;
    testnames)
        echo "Compiling tests/test_names.c -> $OUT_DIR/test_names"
        clang $CFLAGS ./tests/test_names.c -o $OUT_DIR/test_names
        ;;
    testnamearray)
        echo "Compiling tests/test_namearray.c -> $OUT_DIR/test_namearray"
        clang $CFLAGS ./tests/test_namearray.c -o $OUT_DIR/test_namearray
        ;;
    teststring)
        echo "Compiling tests/test_string.c -> $OUT_DIR/test_string"
        clang $CFLAGS ./tests/test_string.c -o $OUT_DIR/test_string
        ;;
    testlinkedlist)
        echo "Compiling tests/test_linkedlist.c -> $OUT_DIR/test_linkedlist"
        clang $CFLAGS ./tests/test_linkedlist.c -o $OUT_DIR/test_linkedlist
        ;;
    testregistration)
        echo "Compiling tests/test_registration.c -> $OUT_DIR/test_registration"
        clang $CFLAGS ./tests/test_registration.c -o $OUT_DIR/test_registration
        ;;
    testarena)
        echo "Compiling tests/test_arena.c -> $OUT_DIR/test_arena"
        clang $CFLAGS ./tests/test_arena.c -o $OUT_DIR/test_arena
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
