#!/bin/bash

status=true

# ---------------------------------------------------

./gauss AB.csv >X.csv.out

if cmp -s X.csv X.csv.out; then
  echo Gauss ok!
else
  echo Gauss failed! >&2
  status=false
fi

# ---------------------------------------------------

$status
