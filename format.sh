#!/bin/sh

# This script finds all files with .cc or .h extensions in the current directory 
# and its subdirectories, and applies the clang-format tool to format them according
# to the style defined in a configuration file named ".clang-format" located in the 
# root directory of the project. The "-i" option overwrites the original files with 
# the formatted version.
find . -name "*.cc" -o -name "*.h" | xargs clang-format -style=file -i
