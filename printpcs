#!/bin/sh

# Decode the symbols from a panic EIP list

# Find a working addr2line
for p in i386-jos-elf-addr2line addr2line; do
    if which $p 2>&1 >/dev/null && \
       $p -h 2>&1 | grep -q '\belf32-i386\b'; then
        break
    fi
done

# Enable as much pretty-printing as this addr2line can do
$p $($p -h | grep ' -[aipsf] ' | awk '{print $1}') -e kernel "$@"
