#!/bin/bash

CHECKS='-*,clang-analyzer-*,performance-*,bugprone-*'
FILES=$(git ls-files '*.cpp' '*.cxx' '*.cc' | tr '\n' ' ')

if [[ -z "$FILES" ]]; then
  echo "No source files to analyze."
  exit 0
fi

set -eo pipefail

for f in $FILES; do
  echo "Running clang-tidy on $f"
  clang-tidy -p .  -checks="$CHECKS" "$f" --extra-arg="-isystem${PWD}/../external/eigen" --extra-arg="-isystem${PWD}/../external/lazycsv/include" --quiet 2>&1
done
