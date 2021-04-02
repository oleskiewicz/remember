#!/bin/sh -e
#
# forget

REMEMBER_DIR="${REMEMBER_DIR:-$XDG_CACHE_HOME/remember}"

tmp="$(mktemp)"
trap 'rm $tmp' EXIT

offset="${1:--v-1m}"
touch -t "$(date "$offset" "+%Y%m%d%H%M")" "$tmp"
find "$REMEMBER_DIR" -print0 -type f -not -newer "$tmp"| xargs -0 rm
