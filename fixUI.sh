#!/bin/bash

cd "$(dirname "$0")"

FILE_LIST=( $(find . -name "*.ui" -type f) )

for i in "${FILE_LIST[@]}"; do
  OLD_SUM="$(sha512sum "$i")"
  sed -i ':a;N;$!ba;s/\(iconset[ ]\+theme[^>]\+\)>[\n ]*<normaloff>.<\/normaloff>.<\/iconset>/\1\/>/g' "$i"
  if [[ "$OLD_SUM" != "$(sha512sum "$i")" ]]; then
    echo "CHANGED $i"
  fi
done
