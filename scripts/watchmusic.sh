#!/bin/sh

inotifywait -q -m -r --format "%e:%w%f" -e delete,create,close_write,moved_to,moved_from $1 | grep --line-buffered '.flac$\|.mp3$' | grep --line-buffered 'CLOSE_WRITE\|CREATE\|DELETE\|MOVED_TO\|MOVED_FROM' | /home/crunch/projects/musicmaniac-build/musiccmd

