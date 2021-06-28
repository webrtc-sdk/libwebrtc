#!/bin/sh

find . -name "*.cc" -o -name "*.h" | xargs clang-format -style=file -i
